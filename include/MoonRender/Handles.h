#pragma once

#include <cstdint>

namespace MoonRender {

struct EntityHandle {
    uint32_t id = 0;
};

struct MaterialHandle {
    uint32_t id = 0;
};

struct MeshHandle {
    uint32_t id = 0;
};

struct CameraHandle {
    uint32_t id = 0;
};

inline bool IsValid(EntityHandle handle) { return handle.id != 0; }
inline bool IsValid(MaterialHandle handle) { return handle.id != 0; }
inline bool IsValid(MeshHandle handle) { return handle.id != 0; }
inline bool IsValid(CameraHandle handle) { return handle.id != 0; }

} // namespace MoonRender
