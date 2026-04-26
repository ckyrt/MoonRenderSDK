#pragma once

#include "Handles.h"
#include "Types.h"

namespace MoonRender {

struct CameraDesc {
    Vec3 position = { 0.0f, 2.0f, -6.0f };
    Vec3 target = { 0.0f, 0.0f, 0.0f };
    float fovYDegrees = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 5000.0f;
};

} // namespace MoonRender
