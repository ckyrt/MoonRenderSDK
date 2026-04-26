#pragma once

#include "Camera.h"
#include "Environment.h"
#include "Export.h"
#include "Handles.h"
#include "Lights.h"
#include "Materials.h"
#include "Terrain.h"
#include "Types.h"

#include <cstdint>
#include <memory>

namespace MoonRender {

namespace detail {
struct WorldState;
}

class MOONRENDER_API World {
public:
    MaterialHandle CreateMaterial(const MaterialDesc& desc);
    EntityHandle CreateEntity(const Transform& transform = {});
    EntityHandle CreatePrimitive(const Transform& transform, MaterialHandle material);
    EntityHandle CreateGround(const GroundDesc& desc);
    EntityHandle CreateFloor(const FloorDesc& desc);
    EntityHandle CreateWall(const WallDesc& desc);
    EntityHandle CreateWaterPlane(const WaterPlaneDesc& desc);
    EntityHandle CreateTerrain(const TerrainDesc& desc);
    EntityHandle CreateSky(const SkyDesc& desc);
    EntityHandle CreateDirectionalLight(const DirectionalLightDesc& desc);
    EntityHandle CreatePointLight(const PointLightDesc& desc);
    EntityHandle CreateSpotLight(const SpotLightDesc& desc);
    CameraHandle CreateCamera(const CameraDesc& desc);
    void SetMainCamera(CameraHandle camera);
    void SetWeather(const WeatherDesc& desc);
    void SetWind(const WindDesc& desc);

private:
    std::shared_ptr<detail::WorldState> m_state;

    explicit World(std::shared_ptr<detail::WorldState> state);

    friend class Runtime;
};

} // namespace MoonRender
