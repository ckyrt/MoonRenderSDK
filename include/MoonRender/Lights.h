#pragma once

#include "Types.h"

namespace MoonRender {

struct DirectionalLightDesc {
    Vec3 direction = { -0.35f, -0.8f, 0.25f };
    Color color = { 1.0f, 0.96f, 0.84f, 1.0f };
    float intensity = 10.0f;
    bool castShadows = true;
};

struct PointLightDesc {
    Vec3 position = {};
    Color color = {};
    float intensity = 8.0f;
    float range = 12.0f;
    bool castShadows = false;
};

struct SpotLightDesc {
    Vec3 position = {};
    Vec3 target = { 0.0f, 0.0f, 1.0f };
    Color color = {};
    float intensity = 8.0f;
    float range = 20.0f;
    float innerConeDeg = 20.0f;
    float outerConeDeg = 35.0f;
    bool castShadows = false;
};

} // namespace MoonRender
