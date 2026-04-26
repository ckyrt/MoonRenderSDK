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

namespace MoonRender {

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
    uint32_t m_nextEntityId = 1;
    uint32_t m_nextMaterialId = 1;
    uint32_t m_nextMeshId = 1;
    uint32_t m_nextCameraId = 1;
};

} // namespace MoonRender
