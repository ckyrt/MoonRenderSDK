#include "AssetPaths.h"

namespace Moon {
namespace Assets {

namespace {

std::string g_projectRootPath;
std::string g_assetRootPath = "assets";
std::string g_textureRootPath;

} // namespace

void ConfigureAssetRoots(
    const std::string& projectRoot,
    const std::string& assetRoot,
    const std::string& textureRoot)
{
    g_projectRootPath = NormalizeSlashes(projectRoot);
    g_assetRootPath = NormalizeSlashes(assetRoot.empty() ? "assets" : assetRoot);
    g_textureRootPath = NormalizeSlashes(textureRoot);

    if (g_projectRootPath.empty()) {
        const size_t lastSlash = g_assetRootPath.find_last_of('/');
        if (lastSlash != std::string::npos) {
            g_projectRootPath = g_assetRootPath.substr(0, lastSlash);
        }
    }
}

const std::string& GetProjectRootPath()
{
    return g_projectRootPath;
}

const std::string& GetAssetRootPath()
{
    return g_assetRootPath;
}

const std::string& GetTextureRootPath()
{
    return g_textureRootPath;
}

} // namespace Assets
} // namespace Moon
