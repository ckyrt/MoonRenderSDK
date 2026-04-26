#pragma once

#include "Handles.h"
#include "Types.h"

namespace MoonRender {

enum class MaterialPreset {
    None,
    Concrete,
    ConcreteFloor,
    Rock,
    Brick,
    Stone,
    Plaster,
    TileCeramic,
    Wood,
    WoodFloor,
    Metal,
    Glass,
    Fabric,
    Leather,
    Carpet,
    Plastic,
    Rubber,
    Water,
    Foliage
};

enum class MappingMode {
    UV,
    Triplanar
};

enum class ShadingModel {
    DefaultLit,
    Water,
    Foliage
};

struct MaterialDesc {
    MaterialPreset preset = MaterialPreset::None;
    MappingMode mapping = MappingMode::UV;
    ShadingModel shading = ShadingModel::DefaultLit;
    Color baseColor = {};
    float metallic = 0.0f;
    float roughness = 0.5f;
    float opacity = 1.0f;
    float tiling = 1.0f;
};

} // namespace MoonRender
