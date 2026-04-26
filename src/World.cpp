#include <MoonRender/World.h>

#include "SdkInternal.h"

#include "moon/engine/core/Geometry/MeshGenerator.h"
#include "moon/engine/core/Math/Quaternion.h"
#include "moon/engine/core/Scene/Light.h"
#include "moon/engine/core/Scene/Material.h"
#include "moon/engine/core/Scene/MeshRenderer.h"
#include "moon/engine/core/Scene/Skybox.h"
#include "moon/engine/environment/EnvironmentTypes.h"
#include "moon/engine/terrain/ProceduralTerrainGenerator.h"
#include "moon/engine/terrain/TerrainComponent.h"
#include "moon/engine/terrain/TerrainVisualBuilder.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <memory>
#include <string>

namespace MoonRender {

namespace {

Moon::Vector3 ToMoon(const Vec3& value)
{
    return Moon::Vector3(value.x, value.y, value.z);
}

Moon::Vector3 ToMoon(const Color& value)
{
    return Moon::Vector3(value.r, value.g, value.b);
}

Moon::MaterialPreset ToMoon(MaterialPreset preset)
{
    switch (preset) {
    case MaterialPreset::Concrete: return Moon::MaterialPreset::Concrete;
    case MaterialPreset::ConcreteFloor: return Moon::MaterialPreset::ConcreteFloor;
    case MaterialPreset::Rock: return Moon::MaterialPreset::Rock;
    case MaterialPreset::Brick: return Moon::MaterialPreset::Brick;
    case MaterialPreset::Stone: return Moon::MaterialPreset::Stone;
    case MaterialPreset::Plaster: return Moon::MaterialPreset::Plaster;
    case MaterialPreset::TileCeramic: return Moon::MaterialPreset::TileCeramic;
    case MaterialPreset::Wood: return Moon::MaterialPreset::Wood;
    case MaterialPreset::WoodFloor: return Moon::MaterialPreset::WoodFloor;
    case MaterialPreset::Metal: return Moon::MaterialPreset::Metal;
    case MaterialPreset::Glass: return Moon::MaterialPreset::Glass;
    case MaterialPreset::Fabric: return Moon::MaterialPreset::Fabric;
    case MaterialPreset::Leather: return Moon::MaterialPreset::Leather;
    case MaterialPreset::Carpet: return Moon::MaterialPreset::Carpet;
    case MaterialPreset::Plastic: return Moon::MaterialPreset::Plastic;
    case MaterialPreset::Rubber: return Moon::MaterialPreset::Rubber;
    case MaterialPreset::Water: return Moon::MaterialPreset::Glass;
    case MaterialPreset::Foliage: return Moon::MaterialPreset::Fabric;
    case MaterialPreset::None:
    default:
        return Moon::MaterialPreset::None;
    }
}

Moon::MappingMode ToMoon(MappingMode mapping)
{
    return mapping == MappingMode::Triplanar ? Moon::MappingMode::Triplanar : Moon::MappingMode::UV;
}

Moon::ShadingModel ToMoon(ShadingModel shading)
{
    return shading == ShadingModel::Water ? Moon::ShadingModel::Water : Moon::ShadingModel::DefaultLit;
}

Moon::WeatherType ToMoon(WeatherType weather)
{
    switch (weather) {
    case WeatherType::Cloudy: return Moon::WeatherType::Cloudy;
    case WeatherType::Rain: return Moon::WeatherType::Rain;
    case WeatherType::Snow: return Moon::WeatherType::Snow;
    case WeatherType::Fog: return Moon::WeatherType::Fog;
    case WeatherType::Storm: return Moon::WeatherType::Storm;
    case WeatherType::Clear:
    default:
        return Moon::WeatherType::Clear;
    }
}

std::string NormalizePath(const char* path)
{
    return path != nullptr ? std::filesystem::path(path).make_preferred().string() : std::string();
}

std::string JoinPath(const std::string& left, const std::string& right)
{
    if (left.empty()) {
        return right;
    }
    return (std::filesystem::path(left) / right).make_preferred().string();
}

std::string ResolveConventionTexture(
    const MaterialTextureConvention& convention,
    const std::string& textureRoot,
    const char* suffix)
{
    if (textureRoot.empty() || convention.materialFolder == nullptr || suffix == nullptr || convention.extension == nullptr) {
        return {};
    }

    const std::filesystem::path folder = std::filesystem::path(textureRoot) / convention.materialFolder;
    if (!std::filesystem::exists(folder) || !std::filesystem::is_directory(folder)) {
        return {};
    }

    const std::string expectedEnding = std::string(suffix) + convention.extension;
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        const std::string filename = entry.path().filename().string();
        if (filename.size() >= expectedEnding.size() &&
            filename.compare(filename.size() - expectedEnding.size(), expectedEnding.size(), expectedEnding) == 0) {
            std::filesystem::path matched = entry.path();
            return matched.make_preferred().string();
        }
    }

    return {};
}

Moon::EnvironmentComponent* EnsureEnvironment(detail::WorldState& state)
{
    if (state.environment != nullptr) {
        return state.environment;
    }

    Moon::SceneNode* node = state.scene->CreateNode("Environment");
    state.environment = node->AddComponent<Moon::EnvironmentComponent>();
    return state.environment;
}

Moon::SceneNode* CreateNode(detail::WorldState& state, const char* name, const Transform& transform)
{
    Moon::SceneNode* node = state.scene->CreateNode(name != nullptr ? name : "Entity");
    node->GetTransform()->SetLocalPosition(ToMoon(transform.position));
    node->GetTransform()->SetLocalRotation(transform.rotationEulerDeg.x == 0.0f &&
            transform.rotationEulerDeg.y == 0.0f && transform.rotationEulerDeg.z == 0.0f
        ? Moon::Vector3(0.0f, 0.0f, 0.0f)
        : ToMoon(transform.rotationEulerDeg));
    node->GetTransform()->SetLocalScale(ToMoon(transform.scale));
    return node;
}

void ApplyMaterial(
    detail::WorldState& state,
    Moon::Material* material,
    const MaterialDesc& desc)
{
    if (material == nullptr) {
        return;
    }

    material->SetMaterialPreset(ToMoon(desc.preset));
    material->SetMappingMode(ToMoon(desc.mapping));
    material->SetShadingModel(ToMoon(desc.shading));
    material->SetBaseColor(ToMoon(desc.baseColor));
    material->SetMetallic(desc.metallic);
    material->SetRoughness(desc.roughness);
    material->SetOpacity(desc.opacity);
    material->SetTriplanarTiling(desc.tiling);

    const std::string textureRoot = state.runtime ? state.runtime->textureRoot : std::string();

    const std::string albedo = desc.textures.albedo != nullptr
        ? NormalizePath(desc.textures.albedo)
        : ResolveConventionTexture(desc.textureConvention, textureRoot, desc.textureConvention.albedoSuffix);
    const std::string normal = desc.textures.normal != nullptr
        ? NormalizePath(desc.textures.normal)
        : ResolveConventionTexture(desc.textureConvention, textureRoot, desc.textureConvention.normalSuffix);
    const std::string ao = desc.textures.ambientOcclusion != nullptr
        ? NormalizePath(desc.textures.ambientOcclusion)
        : ResolveConventionTexture(desc.textureConvention, textureRoot, desc.textureConvention.ambientOcclusionSuffix);
    const std::string roughness = desc.textures.roughness != nullptr
        ? NormalizePath(desc.textures.roughness)
        : ResolveConventionTexture(desc.textureConvention, textureRoot, desc.textureConvention.roughnessSuffix);
    const std::string metalness = desc.textures.metalness != nullptr
        ? NormalizePath(desc.textures.metalness)
        : ResolveConventionTexture(desc.textureConvention, textureRoot, desc.textureConvention.metalnessSuffix);
    const std::string opacity = desc.textures.opacity != nullptr
        ? NormalizePath(desc.textures.opacity)
        : ResolveConventionTexture(desc.textureConvention, textureRoot, desc.textureConvention.opacitySuffix);

    if (!albedo.empty()) {
        material->SetAlbedoMap(albedo);
    }
    if (!normal.empty()) {
        material->SetNormalMap(normal);
    }
    if (!ao.empty()) {
        material->SetAOMap(ao);
    }
    if (!roughness.empty()) {
        material->SetRoughnessMap(roughness);
    }
    if (!metalness.empty()) {
        material->SetMetalnessMap(metalness);
    }
    if (!opacity.empty()) {
        material->SetOpacity(desc.opacity);
    }

    if (desc.shading == ShadingModel::Foliage) {
        material->SetUseVertexColorTint(true);
        material->SetAlphaCutoff(0.5f);
    }
}

MaterialDesc ResolveMaterial(detail::WorldState& state, MaterialHandle handle, MaterialPreset fallbackPreset)
{
    auto it = state.materials.find(handle.id);
    if (it != state.materials.end()) {
        return it->second;
    }

    MaterialDesc material;
    material.preset = fallbackPreset;
    if (fallbackPreset == MaterialPreset::ConcreteFloor || fallbackPreset == MaterialPreset::Rock || fallbackPreset == MaterialPreset::Brick) {
        material.mapping = MappingMode::Triplanar;
    }
    if (fallbackPreset == MaterialPreset::Water) {
        material.shading = ShadingModel::Water;
        material.opacity = 0.62f;
    }
    if (fallbackPreset == MaterialPreset::Foliage) {
        material.shading = ShadingModel::Foliage;
    }
    return material;
}

EntityHandle RegisterEntity(detail::WorldState& state, Moon::SceneNode* node)
{
    if (node == nullptr) {
        return {};
    }

    const EntityHandle handle{ state.nextEntityId++ };
    state.entities.emplace(handle.id, node);
    return handle;
}

std::shared_ptr<Moon::Mesh> BuildUnitCube(detail::WorldState& state)
{
    return state.meshManager->CreateCube(1.0f, Moon::Vector3(1.0f, 1.0f, 1.0f));
}

} // namespace

World::World(std::shared_ptr<detail::WorldState> state)
    : m_state(std::move(state))
{
}

MaterialHandle World::CreateMaterial(const MaterialDesc& desc)
{
    if (!m_state) {
        return {};
    }

    const MaterialHandle handle{ m_state->nextMaterialId++ };
    m_state->materials.emplace(handle.id, desc);
    return handle;
}

EntityHandle World::CreateEntity(const Transform& transform)
{
    if (!m_state) {
        return {};
    }

    return RegisterEntity(*m_state, CreateNode(*m_state, "Entity", transform));
}

EntityHandle World::CreatePrimitive(const Transform& transform, MaterialHandle materialHandle)
{
    if (!m_state) {
        return {};
    }

    Moon::SceneNode* node = CreateNode(*m_state, "Primitive", transform);
    Moon::MeshRenderer* renderer = node->AddComponent<Moon::MeshRenderer>();
    renderer->SetMesh(BuildUnitCube(*m_state));
    ApplyMaterial(*m_state, node->AddComponent<Moon::Material>(), ResolveMaterial(*m_state, materialHandle, MaterialPreset::Concrete));
    return RegisterEntity(*m_state, node);
}

EntityHandle World::CreateGround(const GroundDesc& desc)
{
    FloorDesc floor;
    floor.center = { 0.0f, 0.0f, 0.0f };
    floor.size = desc.size;
    floor.material = desc.material;
    return CreateFloor(floor);
}

EntityHandle World::CreateFloor(const FloorDesc& desc)
{
    if (!m_state) {
        return {};
    }

    Moon::SceneNode* node = CreateNode(*m_state, "Floor", {});
    node->GetTransform()->SetLocalPosition(ToMoon(desc.center));

    Moon::MeshRenderer* renderer = node->AddComponent<Moon::MeshRenderer>();
    renderer->SetMesh(m_state->meshManager->CreatePlane(
        desc.size.x,
        desc.size.y,
        1,
        1,
        Moon::Vector3(1.0f, 1.0f, 1.0f)));

    ApplyMaterial(*m_state, node->AddComponent<Moon::Material>(), ResolveMaterial(*m_state, desc.material, MaterialPreset::ConcreteFloor));
    return RegisterEntity(*m_state, node);
}

EntityHandle World::CreateWall(const WallDesc& desc)
{
    if (!m_state) {
        return {};
    }

    const Moon::Vector3 start = ToMoon(desc.start);
    const Moon::Vector3 end = ToMoon(desc.end);
    const Moon::Vector3 delta = end - start;
    const float length = std::max(0.001f, std::sqrt(delta.x * delta.x + delta.z * delta.z));
    const float yawDegrees = std::atan2(delta.z, delta.x) * 180.0f / 3.14159265358979323846f;

    Moon::SceneNode* node = CreateNode(*m_state, "Wall", {});
    node->GetTransform()->SetLocalPosition(Moon::Vector3(
        (start.x + end.x) * 0.5f,
        std::min(start.y, end.y) + desc.height * 0.5f,
        (start.z + end.z) * 0.5f));
    node->GetTransform()->SetLocalRotation(Moon::Vector3(0.0f, -yawDegrees, 0.0f));
    node->GetTransform()->SetLocalScale(Moon::Vector3(length, desc.height, desc.thickness));

    Moon::MeshRenderer* renderer = node->AddComponent<Moon::MeshRenderer>();
    renderer->SetMesh(BuildUnitCube(*m_state));
    ApplyMaterial(*m_state, node->AddComponent<Moon::Material>(), ResolveMaterial(*m_state, desc.material, MaterialPreset::Brick));
    return RegisterEntity(*m_state, node);
}

EntityHandle World::CreateWaterPlane(const WaterPlaneDesc& desc)
{
    if (!m_state) {
        return {};
    }

    Moon::SceneNode* node = CreateNode(*m_state, "Water", {});
    node->GetTransform()->SetLocalPosition(ToMoon(desc.center));

    Moon::MeshRenderer* renderer = node->AddComponent<Moon::MeshRenderer>();
    renderer->SetMesh(m_state->meshManager->CreatePlane(
        desc.size.x,
        desc.size.y,
        8,
        8,
        Moon::Vector3(1.0f, 1.0f, 1.0f)));

    Moon::Material* material = node->AddComponent<Moon::Material>();
    MaterialDesc waterMaterial;
    waterMaterial.preset = MaterialPreset::Water;
    waterMaterial.shading = ShadingModel::Water;
    waterMaterial.baseColor = desc.color;
    waterMaterial.roughness = 0.06f;
    waterMaterial.opacity = desc.color.a;
    ApplyMaterial(*m_state, material, waterMaterial);
    return RegisterEntity(*m_state, node);
}

EntityHandle World::CreateTerrain(const TerrainDesc& desc)
{
    if (!m_state) {
        return {};
    }

    Moon::TerrainGenerationSettings settings;
    settings.resolution = desc.resolution;
    settings.worldWidth = desc.worldWidth;
    settings.worldDepth = desc.worldDepth;
    settings.heightScale = desc.heightScale;
    settings.seed = desc.seed;
    settings.hasOcean = desc.createOcean;

    Moon::TerrainGenerationResult generation = Moon::ProceduralTerrainGenerator::CreateOpenWorldLandscape(settings);

    Moon::SceneNode* root = m_state->scene->CreateNode("TerrainRoot");

    Moon::SceneNode* terrainNode = m_state->scene->CreateNode("Terrain");
    terrainNode->SetParent(root);
    terrainNode->AddComponent<Moon::MeshRenderer>()->SetMesh(Moon::TerrainVisualBuilder::BuildTerrainMesh(generation, settings));
    ApplyMaterial(*m_state, terrainNode->AddComponent<Moon::Material>(), ResolveMaterial(*m_state, {}, MaterialPreset::Rock));

    Moon::TerrainComponent* terrainComponent = terrainNode->AddComponent<Moon::TerrainComponent>();
    Moon::TerrainProfile profile;
    profile.worldWidth = settings.worldWidth;
    profile.worldDepth = settings.worldDepth;
    profile.heightScale = settings.heightScale;
    terrainComponent->SetProfile(profile);
    terrainComponent->SetData(generation.terrainData);

    if (desc.createRivers) {
        Moon::SceneNode* riversNode = m_state->scene->CreateNode("Rivers");
        riversNode->SetParent(root);
        riversNode->AddComponent<Moon::MeshRenderer>()->SetMesh(Moon::TerrainVisualBuilder::BuildRiverMesh(generation, settings));

        MaterialDesc riverMaterial;
        riverMaterial.preset = MaterialPreset::Water;
        riverMaterial.shading = ShadingModel::Water;
        riverMaterial.baseColor = { 0.10f, 0.32f, 0.54f, 0.62f };
        riverMaterial.opacity = 0.62f;
        ApplyMaterial(*m_state, riversNode->AddComponent<Moon::Material>(), riverMaterial);
    }

    if (desc.createOcean && settings.hasOcean) {
        Moon::SceneNode* oceanNode = m_state->scene->CreateNode("Ocean");
        oceanNode->SetParent(root);
        oceanNode->AddComponent<Moon::MeshRenderer>()->SetMesh(Moon::TerrainVisualBuilder::BuildOceanMesh(generation, settings));

        MaterialDesc oceanMaterial;
        oceanMaterial.preset = MaterialPreset::Water;
        oceanMaterial.shading = ShadingModel::Water;
        oceanMaterial.baseColor = { 0.06f, 0.24f, 0.44f, 0.68f };
        oceanMaterial.opacity = 0.68f;
        ApplyMaterial(*m_state, oceanNode->AddComponent<Moon::Material>(), oceanMaterial);
    }

    if (desc.createGrass) {
        Moon::SceneNode* grassNode = m_state->scene->CreateNode("Grass");
        grassNode->SetParent(root);
        grassNode->AddComponent<Moon::MeshRenderer>()->SetMesh(
            Moon::TerrainVisualBuilder::BuildGrassMesh(generation.terrainData, generation, settings));

        MaterialDesc grassMaterial;
        grassMaterial.preset = MaterialPreset::Foliage;
        grassMaterial.shading = ShadingModel::Foliage;
        grassMaterial.baseColor = { 0.18f, 0.42f, 0.18f, 1.0f };
        grassMaterial.roughness = 0.9f;
        ApplyMaterial(*m_state, grassNode->AddComponent<Moon::Material>(), grassMaterial);
    }

    return RegisterEntity(*m_state, root);
}

EntityHandle World::CreateSky(const SkyDesc& desc)
{
    if (!m_state) {
        return {};
    }

    Moon::SceneNode* node = CreateNode(*m_state, "Sky", {});
    Moon::Skybox* skybox = node->AddComponent<Moon::Skybox>();
    skybox->SetIntensity(desc.intensity);
    skybox->SetEnableIBL(true);

    if (desc.type == SkyType::Procedural) {
        skybox->SetType(Moon::Skybox::Type::Procedural);
    } else if (desc.environmentMapPath != nullptr) {
        skybox->LoadEnvironmentMap(desc.environmentMapPath);
    }

    return RegisterEntity(*m_state, node);
}

EntityHandle World::CreateDirectionalLight(const DirectionalLightDesc& desc)
{
    if (!m_state) {
        return {};
    }

    Moon::SceneNode* node = CreateNode(*m_state, "DirectionalLight", {});
    node->GetTransform()->LookAt(ToMoon(desc.direction));

    Moon::Light* light = node->AddComponent<Moon::Light>();
    light->SetType(Moon::Light::Type::Directional);
    light->SetColor(ToMoon(desc.color));
    light->SetIntensity(desc.intensity);
    light->SetCastShadows(desc.castShadows);
    return RegisterEntity(*m_state, node);
}

EntityHandle World::CreatePointLight(const PointLightDesc& desc)
{
    if (!m_state) {
        return {};
    }

    Moon::SceneNode* node = CreateNode(*m_state, "PointLight", {});
    node->GetTransform()->SetLocalPosition(ToMoon(desc.position));

    Moon::Light* light = node->AddComponent<Moon::Light>();
    light->SetType(Moon::Light::Type::Point);
    light->SetColor(ToMoon(desc.color));
    light->SetIntensity(desc.intensity);
    light->SetRange(desc.range);
    light->SetCastShadows(desc.castShadows);
    return RegisterEntity(*m_state, node);
}

EntityHandle World::CreateSpotLight(const SpotLightDesc& desc)
{
    if (!m_state) {
        return {};
    }

    Moon::SceneNode* node = CreateNode(*m_state, "SpotLight", {});
    node->GetTransform()->SetLocalPosition(ToMoon(desc.position));
    node->GetTransform()->LookAt(ToMoon(desc.target));

    Moon::Light* light = node->AddComponent<Moon::Light>();
    light->SetType(Moon::Light::Type::Spot);
    light->SetColor(ToMoon(desc.color));
    light->SetIntensity(desc.intensity);
    light->SetRange(desc.range);
    light->SetSpotAngles(desc.innerConeDeg, desc.outerConeDeg);
    light->SetCastShadows(desc.castShadows);
    return RegisterEntity(*m_state, node);
}

CameraHandle World::CreateCamera(const CameraDesc& desc)
{
    if (!m_state) {
        return {};
    }

    const CameraHandle handle{ m_state->nextCameraId++ };
    const float aspect = m_state->runtime && m_state->runtime->desc.height != 0
        ? static_cast<float>(m_state->runtime->desc.width) / static_cast<float>(m_state->runtime->desc.height)
        : (16.0f / 9.0f);
    auto camera = std::make_unique<Moon::PerspectiveCamera>(
        desc.fovYDegrees,
        aspect,
        desc.nearPlane,
        desc.farPlane);
    camera->SetPosition(ToMoon(desc.position));
    camera->LookAt(ToMoon(desc.target));

    if (m_state->mainCamera == nullptr) {
        m_state->mainCamera = camera.get();
    }

    m_state->cameras.emplace(handle.id, std::move(camera));
    return handle;
}

void World::SetMainCamera(CameraHandle camera)
{
    if (!m_state) {
        return;
    }

    auto it = m_state->cameras.find(camera.id);
    if (it != m_state->cameras.end()) {
        m_state->mainCamera = it->second.get();
    }
}

void World::SetWeather(const WeatherDesc& desc)
{
    if (!m_state) {
        return;
    }

    Moon::EnvironmentComponent* environment = EnsureEnvironment(*m_state);
    environment->SetTimeOfDay(desc.timeOfDayHours);
    environment->SetWeather(ToMoon(desc.type), desc.transitionSeconds);
}

void World::SetWind(const WindDesc& desc)
{
    if (!m_state) {
        return;
    }

    Moon::EnvironmentComponent* environment = EnsureEnvironment(*m_state);
    Moon::EnvironmentProfile profile = environment->GetProfile();
    profile.baseWindSpeed = desc.speed;
    profile.baseWindGustStrength = desc.gustStrength;
    environment->SetProfile(profile);

    Moon::EnvironmentState state = environment->GetState();
    state.wind.direction = ToMoon(desc.direction);
    state.wind.speed = desc.speed;
    state.wind.gustStrength = desc.gustStrength;
    environment->GetSystem().SetState(state);
}

} // namespace MoonRender
