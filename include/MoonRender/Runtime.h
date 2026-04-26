#pragma once

#include "Export.h"
#include "Types.h"
#include "Window.h"
#include "World.h"

#include <memory>

namespace MoonRender {

namespace detail {
struct RuntimeState;
}

class MOONRENDER_API Runtime {
public:
    bool Initialize(const RuntimeDesc& desc = {});
    void Shutdown();
    bool PollEvents();
    void Tick(float deltaTimeSeconds = 0.0f);
    void Render(World& world);
    void Resize(unsigned width, unsigned height);
    void* GetWindowHandle() const;
    World CreateWorld();

private:
    std::shared_ptr<detail::RuntimeState> m_state;
    Window m_window;
    bool m_initialized = false;
};

} // namespace MoonRender
