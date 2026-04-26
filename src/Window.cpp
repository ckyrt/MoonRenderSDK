#include <MoonRender/Window.h>

namespace MoonRender {

bool Window::Create(const RuntimeDesc& desc)
{
    m_width = desc.width;
    m_height = desc.height;
    m_shouldClose = false;
    return true;
}

void Window::Destroy()
{
    m_nativeHandle = nullptr;
    m_width = 0;
    m_height = 0;
    m_shouldClose = true;
}

bool Window::PollEvents()
{
    return !m_shouldClose;
}

bool Window::ShouldClose() const
{
    return m_shouldClose;
}

void* Window::GetNativeHandle() const
{
    return m_nativeHandle;
}

unsigned Window::GetWidth() const
{
    return m_width;
}

unsigned Window::GetHeight() const
{
    return m_height;
}

} // namespace MoonRender
