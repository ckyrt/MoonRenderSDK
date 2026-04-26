# Migrated Moon Runtime Source

This folder contains the first copied runtime implementation payload from Moon.

It is intentionally kept separate from the public SDK facade while the migration is in progress. The final direction is:

1. Keep `include/MoonRender/*` as the only public API.
2. Move reusable runtime implementation from `src/moon/engine/*` into normal SDK modules.
3. Wire public handles to internal scene/material/terrain/render objects.
4. Remove hard-coded Moon paths and editor/tool dependencies.
5. Build all runtime implementation into `MoonRenderSDK.dll`.

Do not expose `SceneNode*`, `DiligentRenderer*`, or Moon editor-specific types in public headers.
