#pragma once

#include <cstdint>

namespace MoonRender {

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct Color {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
};

struct Transform {
    Vec3 position = {};
    Vec3 rotationEulerDeg = {};
    Vec3 scale = { 1.0f, 1.0f, 1.0f };
};

struct RuntimeDesc {
    const char* appName = "MoonRender";
    uint32_t width = 1280;
    uint32_t height = 720;
    const char* assetRoot = "assets";
};

} // namespace MoonRender
