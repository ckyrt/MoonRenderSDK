#pragma once

#include <MoonRender/Environment.h>
#include <MoonRender/Materials.h>
#include <MoonRender/Runtime.h>

#include "moon/engine/core/Assets/MeshManager.h"
#include "moon/engine/core/Camera/PerspectiveCamera.h"
#include "moon/engine/core/Scene/Scene.h"
#include "moon/engine/environment/EnvironmentComponent.h"
#include "moon/engine/render/diligent/DiligentRenderer.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace MoonRender::detail {

struct WorldState;

struct RuntimeState {
    RuntimeDesc desc = {};
    std::string assetRoot;
    std::string textureRoot;
    std::unique_ptr<DiligentRenderer> renderer;
    std::vector<std::weak_ptr<WorldState>> worlds;
};

struct WorldState {
    explicit WorldState(std::shared_ptr<RuntimeState> runtimeState)
        : runtime(std::move(runtimeState))
        , scene(std::make_unique<Moon::Scene>("MoonRenderSDK Scene"))
        , meshManager(std::make_unique<Moon::MeshManager>())
    {
    }

    std::shared_ptr<RuntimeState> runtime;
    std::unique_ptr<Moon::Scene> scene;
    std::unique_ptr<Moon::MeshManager> meshManager;
    std::unordered_map<uint32_t, MaterialDesc> materials;
    std::unordered_map<uint32_t, Moon::SceneNode*> entities;
    std::unordered_map<uint32_t, std::unique_ptr<Moon::PerspectiveCamera>> cameras;
    Moon::PerspectiveCamera* mainCamera = nullptr;
    Moon::EnvironmentComponent* environment = nullptr;
    uint32_t nextEntityId = 1;
    uint32_t nextMaterialId = 1;
    uint32_t nextCameraId = 1;
};

} // namespace MoonRender::detail
