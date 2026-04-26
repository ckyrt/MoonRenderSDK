#pragma once

#include "Types.h"

namespace MoonRender {

enum class SkyType {
    None,
    Procedural,
    EquirectangularHDR,
    Cubemap
};

enum class WeatherType {
    Clear,
    Cloudy,
    Rain,
    Snow,
    Fog,
    Storm
};

struct SkyDesc {
    SkyType type = SkyType::Procedural;
    float intensity = 1.0f;
    const char* environmentMapPath = nullptr;
};

struct WeatherDesc {
    WeatherType type = WeatherType::Clear;
    float timeOfDayHours = 12.0f;
    float transitionSeconds = 0.0f;
};

struct WindDesc {
    Vec3 direction = { 1.0f, 0.0f, 0.0f };
    float speed = 2.0f;
    float gustStrength = 0.15f;
};

} // namespace MoonRender
