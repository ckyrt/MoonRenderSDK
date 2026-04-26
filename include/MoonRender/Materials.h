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

enum class NormalMapConvention {
    DirectX,
    OpenGL
};

struct TextureSetDesc {
    const char* albedo = nullptr;
    const char* normal = nullptr;
    const char* ambientOcclusion = nullptr;
    const char* roughness = nullptr;
    const char* metalness = nullptr;
    const char* opacity = nullptr;
};

struct MaterialTextureConvention {
    const char* materialFolder = nullptr;
    const char* albedoSuffix = "_Color";
    const char* normalSuffix = "_NormalDX";
    const char* ambientOcclusionSuffix = "_AmbientOcclusion";
    const char* roughnessSuffix = "_Roughness";
    const char* metalnessSuffix = "_Metalness";
    const char* opacitySuffix = "_Opacity";
    const char* extension = ".png";
    NormalMapConvention normalConvention = NormalMapConvention::DirectX;
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
    TextureSetDesc textures = {};
    MaterialTextureConvention textureConvention = {};
};

} // namespace MoonRender
