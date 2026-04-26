#pragma once

#include <string>

namespace Moon {
namespace Assets {

void ConfigureAssetRoots(
    const std::string& projectRoot,
    const std::string& assetRoot,
    const std::string& textureRoot = std::string());

const std::string& GetProjectRootPath();
const std::string& GetAssetRootPath();
const std::string& GetTextureRootPath();

inline bool IsAbsolutePath(const std::string& path)
{
    return path.size() > 1 && path[1] == ':';
}

inline std::string NormalizeSlashes(std::string path)
{
    for (char& ch : path) {
        if (ch == '\\') {
            ch = '/';
        }
    }
    return path;
}

inline std::string BuildAssetPath(const std::string& path)
{
    if (path.empty()) {
        return GetAssetRootPath();
    }

    std::string normalized = NormalizeSlashes(path);
    if (IsAbsolutePath(normalized)) {
        return normalized;
    }

    if (normalized.rfind("assets/", 0) == 0) {
        return GetProjectRootPath() + "/" + normalized;
    }

    return GetAssetRootPath() + "/" + normalized;
}

inline std::string BuildBuildingPath(const std::string& path)
{
    return BuildAssetPath("building/" + path);
}

inline std::string BuildObjectPath(const std::string& path)
{
    return BuildAssetPath("objects/" + path);
}

inline std::string BuildShaderPath(const std::string& path)
{
    return BuildAssetPath("shaders/" + path);
}

inline std::string BuildTexturePath(const std::string& path)
{
    std::string normalized = NormalizeSlashes(path);
    if (IsAbsolutePath(normalized) || normalized.rfind("assets/", 0) == 0) {
        return BuildAssetPath(normalized);
    }

    if (normalized.rfind("textures/", 0) == 0) {
        const std::string& textureRoot = GetTextureRootPath();
        if (!textureRoot.empty()) {
            return textureRoot + "/" + normalized.substr(9);
        }
        return BuildAssetPath(normalized);
    }

    const std::string& textureRoot = GetTextureRootPath();
    if (!textureRoot.empty()) {
        return textureRoot + "/" + normalized;
    }

    return BuildAssetPath("textures/" + normalized);
}

} // namespace Assets
} // namespace Moon
