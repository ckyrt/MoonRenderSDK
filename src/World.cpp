#include <MoonRender/World.h>

namespace MoonRender {

MaterialHandle World::CreateMaterial(const MaterialDesc&)
{
    return { m_nextMaterialId++ };
}

EntityHandle World::CreateEntity(const Transform&)
{
    return { m_nextEntityId++ };
}

EntityHandle World::CreatePrimitive(const Transform&, MaterialHandle)
{
    return { m_nextEntityId++ };
}

EntityHandle World::CreateGround(const GroundDesc&)
{
    return { m_nextEntityId++ };
}

EntityHandle World::CreateFloor(const FloorDesc&)
{
    return { m_nextEntityId++ };
}

EntityHandle World::CreateWall(const WallDesc&)
{
    return { m_nextEntityId++ };
}

EntityHandle World::CreateWaterPlane(const WaterPlaneDesc&)
{
    return { m_nextEntityId++ };
}

EntityHandle World::CreateTerrain(const TerrainDesc&)
{
    return { m_nextEntityId++ };
}

EntityHandle World::CreateSky(const SkyDesc&)
{
    return { m_nextEntityId++ };
}

EntityHandle World::CreateDirectionalLight(const DirectionalLightDesc&)
{
    return { m_nextEntityId++ };
}

EntityHandle World::CreatePointLight(const PointLightDesc&)
{
    return { m_nextEntityId++ };
}

EntityHandle World::CreateSpotLight(const SpotLightDesc&)
{
    return { m_nextEntityId++ };
}

CameraHandle World::CreateCamera(const CameraDesc&)
{
    return { m_nextCameraId++ };
}

void World::SetMainCamera(CameraHandle)
{
}

void World::SetWeather(const WeatherDesc&)
{
}

void World::SetWind(const WindDesc&)
{
}

} // namespace MoonRender
