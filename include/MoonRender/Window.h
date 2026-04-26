#pragma once

#include "Export.h"
#include "Types.h"

namespace MoonRender {

class MOONRENDER_API Window {
public:
    bool Create(const RuntimeDesc& desc);
    void Destroy();
    bool PollEvents();
    bool ShouldClose() const;
    void* GetNativeHandle() const;
    unsigned GetWidth() const;
    unsigned GetHeight() const;

private:
    void* m_nativeHandle = nullptr;
    unsigned m_width = 0;
    unsigned m_height = 0;
    bool m_shouldClose = false;
};

} // namespace MoonRender
