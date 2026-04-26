#include <MoonRender/Runtime.h>

#include "SdkInternal.h"

#include "moon/engine/core/Assets/AssetPaths.h"
#include "moon/engine/core/Logging/Logger.h"
#include "moon/engine/core/Texture/TextureManager.h"
#include "moon/engine/render/SceneRenderer.h"

namespace MoonRender {

bool Runtime::Initialize(const RuntimeDesc& desc)
{
    if (m_initialized) {
        Shutdown();
    }

    m_initialized = m_window.Create(desc);
    if (!m_initialized) {
        return false;
    }

    if (!Moon::Core::Logger::IsInitialized()) {
        Moon::Core::Logger::Init();
    }

    m_state = std::make_shared<detail::RuntimeState>();
    m_state->desc = desc;
    m_state->assetRoot = desc.assetRoot != nullptr ? desc.assetRoot : "assets";
    m_state->textureRoot = desc.textureRoot != nullptr ? desc.textureRoot : "";

    Moon::Assets::ConfigureAssetRoots("", m_state->assetRoot, m_state->textureRoot);
    Moon::TextureManager::SetResourceBasePath(m_state->assetRoot);

    m_state->renderer = std::make_unique<DiligentRenderer>();
    RenderInitParams params{};
    params.windowHandle = m_window.GetNativeHandle();
    params.width = m_window.GetWidth();
    params.height = m_window.GetHeight();

    if (!m_state->renderer->Initialize(params)) {
        m_state->renderer.reset();
        m_state.reset();
        m_window.Destroy();
        m_initialized = false;
        return false;
    }

    return m_initialized;
}

void Runtime::Shutdown()
{
    if (m_state && m_state->renderer) {
        m_state->renderer->Shutdown();
    }
    m_state.reset();
    m_window.Destroy();
    m_initialized = false;
}

bool Runtime::PollEvents()
{
    return m_initialized && m_window.PollEvents();
}

void Runtime::Tick(float deltaTimeSeconds)
{
    if (!m_state) {
        return;
    }

    auto& worlds = m_state->worlds;
    for (auto it = worlds.begin(); it != worlds.end();) {
        std::shared_ptr<detail::WorldState> world = it->lock();
        if (!world) {
            it = worlds.erase(it);
            continue;
        }

        if (world->scene) {
            world->scene->Update(deltaTimeSeconds);
        }
        ++it;
    }
}

void Runtime::Render(World& world)
{
    if (!m_initialized || !m_state || !m_state->renderer || !world.m_state || !world.m_state->scene) {
        return;
    }

    Moon::PerspectiveCamera* camera = world.m_state->mainCamera;
    if (!camera && !world.m_state->cameras.empty()) {
        camera = world.m_state->cameras.begin()->second.get();
        world.m_state->mainCamera = camera;
    }
    if (!camera) {
        return;
    }

    m_state->renderer->BeginFrame();
    Moon::SceneRendererUtils::RenderScene(
        m_state->renderer.get(),
        world.m_state->scene.get(),
        camera,
        world.m_state->environment ? &world.m_state->environment->GetState() : nullptr);
    m_state->renderer->EndFrame();
}

void Runtime::Resize(unsigned width, unsigned height)
{
    if (!m_state || !m_state->renderer) {
        return;
    }

    m_state->renderer->Resize(width, height);

    for (const std::weak_ptr<detail::WorldState>& weakWorld : m_state->worlds) {
        std::shared_ptr<detail::WorldState> world = weakWorld.lock();
        if (!world) {
            continue;
        }

        for (auto& [id, camera] : world->cameras) {
            if (camera) {
                camera->SetAspectRatio(height != 0 ? static_cast<float>(width) / static_cast<float>(height) : (16.0f / 9.0f));
            }
        }
    }
}

void* Runtime::GetWindowHandle() const
{
    return m_window.GetNativeHandle();
}

World Runtime::CreateWorld()
{
    auto state = std::make_shared<detail::WorldState>(m_state);
    if (m_state) {
        m_state->worlds.push_back(state);
    }
    return World(state);
}

} // namespace MoonRender
