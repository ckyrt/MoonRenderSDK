# MoonRenderSDK

MoonRenderSDK is the standalone rendering and 3D scene runtime used by Moon and by external projects that need Moon's rendering features without depending on the Moon editor.

The SDK is intended to own:

- window/runtime/world APIs
- scene creation helpers
- materials and material presets
- texture and shader assets
- PBR rendering
- sky, weather, fog, wind, lights, and shadows
- terrain, water, river, ocean, grass, and shrub runtime visuals

Moon itself should consume this SDK as a dependency. Editor, AI, building authoring, and project-specific tools stay in the main Moon repository.

## Current Status

This repository currently contains the public API skeleton, Visual Studio DLL build layout, and the first migrated Moon runtime source payload under `src/moon`. The next step is wiring that payload behind the public SDK handles and removing Moon-specific paths.

## Layout

```text
include/MoonRender/   Public SDK headers shipped to users
src/                  DLL implementation
assets/               Runtime shaders and material assets shipped to users
examples/             External usage examples
bin/x64/Release/      Built MoonRenderSDK.dll and import library
src/moon/             Migrated Moon runtime source payload being adapted into the DLL
```

## Build

Open `MoonRenderSDK.sln` in Visual Studio 2022 and build `Release|x64`.

The output is:

```text
bin/x64/Release/MoonRenderSDK.dll
bin/x64/Release/MoonRenderSDK.lib
```

External projects do not need to clone this repository. They only need the packaged SDK:

```text
MoonRenderSDK/
  include/
  assets/
  bin/x64/Release/MoonRenderSDK.dll
  bin/x64/Release/MoonRenderSDK.lib
```

Large material textures are distributed as a release asset / texture pack, not committed directly as normal Git blobs.

In another Visual Studio project:

- add `MoonRenderSDK/include` to Additional Include Directories
- add `MoonRenderSDK/bin/x64/Release` to Additional Library Directories
- link `MoonRenderSDK.lib`
- copy `MoonRenderSDK.dll` next to the executable

## Migration Notes

The existing Moon runtime implementation has started moving into `src/moon/engine`:

- `core`: math, camera, input, scene, mesh, geometry, assets, texture
- `render`: renderer interfaces and Diligent renderer implementation
- `environment`: time of day, weather, wind, atmosphere
- `terrain`: terrain runtime, procedural terrain, river/ocean/grass builders
- `assets/shaders`: runtime shader files

These sources are not all wired into the DLL project yet. They still need:

- Diligent dependency packaging inside SDK or a clear binary dependency layout
- asset root configuration using `RuntimeDesc::assetRoot`
- replacement of hard-coded Moon paths
- public-handle wrappers over internal scene/material/light/terrain objects
- material texture pack publishing

## Example

```cpp
#include <MoonRender/MoonRender.h>

int main()
{
    MoonRender::Runtime runtime;
    runtime.Initialize({ "MoonRender Example", 1280, 720 });

    MoonRender::World world = runtime.CreateWorld();
    auto groundMaterial = world.CreateMaterial({
        MoonRender::MaterialPreset::ConcreteFloor,
        MoonRender::MappingMode::Triplanar
    });

    world.CreateSky({ MoonRender::SkyType::Procedural, 1.0f });
    world.SetWeather({ MoonRender::WeatherType::Clear, 12.0f });
    world.CreateGround({ { 100.0f, 100.0f }, groundMaterial });
    world.CreateDirectionalLight({
        { -0.35f, -0.8f, 0.25f },
        { 1.0f, 0.96f, 0.84f },
        10.0f,
        true
    });

    while (runtime.PollEvents()) {
        runtime.Tick();
        runtime.Render(world);
    }

    runtime.Shutdown();
}
```
