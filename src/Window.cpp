#include <MoonRender/Window.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <string>
#endif

namespace MoonRender {

#ifdef _WIN32
namespace {

const wchar_t* kWindowClassName = L"MoonRenderSDKWindow";

struct NativeWindowState {
    void** nativeHandle = nullptr;
    unsigned* width = nullptr;
    unsigned* height = nullptr;
    bool* shouldClose = nullptr;
};

std::wstring ToWideString(const char* text)
{
    if (text == nullptr || text[0] == '\0') {
        return L"MoonRender";
    }

    const int required = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);
    if (required <= 1) {
        return L"MoonRender";
    }

    std::wstring result(static_cast<size_t>(required - 1), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text, -1, result.data(), required);
    return result;
}

LRESULT CALLBACK MoonRenderWindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    NativeWindowState* state = nullptr;

    if (message == WM_NCCREATE) {
        auto* createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
        state = static_cast<NativeWindowState*>(createStruct->lpCreateParams);
        SetWindowLongPtrW(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
    } else {
        state = reinterpret_cast<NativeWindowState*>(GetWindowLongPtrW(windowHandle, GWLP_USERDATA));
    }

    switch (message) {
    case WM_SIZE:
        if (state != nullptr && wParam != SIZE_MINIMIZED) {
            *state->width = static_cast<unsigned>(LOWORD(lParam));
            *state->height = static_cast<unsigned>(HIWORD(lParam));
        }
        return 0;

    case WM_CLOSE:
        if (state != nullptr) {
            *state->shouldClose = true;
        }
        DestroyWindow(windowHandle);
        return 0;

    case WM_DESTROY:
        if (state != nullptr) {
            if (*state->nativeHandle == windowHandle) {
                *state->nativeHandle = nullptr;
            }
            *state->shouldClose = true;
        }
        PostQuitMessage(0);
        return 0;

    case WM_NCDESTROY:
        SetWindowLongPtrW(windowHandle, GWLP_USERDATA, 0);
        delete state;
        return 0;

    default:
        return DefWindowProcW(windowHandle, message, wParam, lParam);
    }
}

bool RegisterMoonRenderWindowClass(HINSTANCE instance)
{
    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = MoonRenderWindowProc;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    windowClass.lpszClassName = kWindowClassName;

    if (RegisterClassExW(&windowClass) != 0) {
        return true;
    }

    return GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

} // namespace

bool Window::Create(const RuntimeDesc& desc)
{
    Destroy();

    const HINSTANCE instance = GetModuleHandleW(nullptr);
    if (!RegisterMoonRenderWindowClass(instance)) {
        return false;
    }

    m_width = desc.width == 0 ? 1280 : desc.width;
    m_height = desc.height == 0 ? 720 : desc.height;
    m_shouldClose = false;

    RECT windowRect{
        0,
        0,
        static_cast<LONG>(m_width),
        static_cast<LONG>(m_height)
    };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    const std::wstring title = ToWideString(desc.appName);
    auto* nativeState = new NativeWindowState{
        &m_nativeHandle,
        &m_width,
        &m_height,
        &m_shouldClose
    };

    HWND windowHandle = CreateWindowExW(
        0,
        kWindowClassName,
        title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        instance,
        nativeState);

    if (windowHandle == nullptr) {
        delete nativeState;
        m_shouldClose = true;
        return false;
    }

    m_nativeHandle = windowHandle;
    ShowWindow(windowHandle, SW_SHOW);
    UpdateWindow(windowHandle);
    return true;
}

void Window::Destroy()
{
    if (m_nativeHandle != nullptr) {
        HWND windowHandle = static_cast<HWND>(m_nativeHandle);
        m_nativeHandle = nullptr;
        DestroyWindow(windowHandle);
    }

    m_nativeHandle = nullptr;
    m_width = 0;
    m_height = 0;
    m_shouldClose = true;
}

bool Window::PollEvents()
{
    MSG message{};
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
        if (message.message == WM_QUIT) {
            m_shouldClose = true;
        }

        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

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
#else
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
#endif

} // namespace MoonRender
