#include <MoonRender/Runtime.h>

namespace MoonRender {

bool Runtime::Initialize(const RuntimeDesc& desc)
{
    m_initialized = m_window.Create(desc);
    return m_initialized;
}

void Runtime::Shutdown()
{
    m_window.Destroy();
    m_initialized = false;
}

bool Runtime::PollEvents()
{
    return m_initialized && m_window.PollEvents();
}

void Runtime::Tick(float)
{
}

void Runtime::Render(World&)
{
}

void Runtime::Resize(unsigned, unsigned)
{
}

void* Runtime::GetWindowHandle() const
{
    return m_window.GetNativeHandle();
}

World Runtime::CreateWorld()
{
    return World();
}

} // namespace MoonRender
