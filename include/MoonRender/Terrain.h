#pragma once

#include "Handles.h"
#include "Types.h"

#include <cstdint>

namespace MoonRender {

struct TerrainDesc {
    uint32_t resolution = 257;
    float worldWidth = 1400.0f;
    float worldDepth = 1400.0f;
    float heightScale = 180.0f;
    uint32_t seed = 1337;
    bool createRivers = true;
    bool createOcean = false;
    bool createGrass = true;
};

struct GroundDesc {
    Vec2 size = { 10.0f, 10.0f };
    MaterialHandle material = {};
};

struct FloorDesc {
    Vec3 center = {};
    Vec2 size = { 10.0f, 10.0f };
    MaterialHandle material = {};
};

struct WallDesc {
    Vec3 start = { -1.0f, 0.0f, 0.0f };
    Vec3 end = { 1.0f, 0.0f, 0.0f };
    float height = 3.0f;
    float thickness = 0.2f;
    MaterialHandle material = {};
};

struct WaterPlaneDesc {
    Vec3 center = {};
    Vec2 size = { 100.0f, 100.0f };
    Color color = { 0.12f, 0.38f, 0.62f, 0.58f };
};

} // namespace MoonRender
