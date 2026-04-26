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

This repository currently contains the public API skeleton and Visual Studio DLL build layout. The next migration step is to move the existing runtime implementation from Moon into this SDK behind the public API.

## Layout

```text
include/MoonRender/   Public SDK headers shipped to users
src/                  DLL implementation
assets/               Runtime shaders and material assets shipped to users
examples/             External usage examples
bin/x64/Release/      Built MoonRenderSDK.dll and import library
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

In another Visual Studio project:

- add `MoonRenderSDK/include` to Additional Include Directories
- add `MoonRenderSDK/bin/x64/Release` to Additional Library Directories
- link `MoonRenderSDK.lib`
- copy `MoonRenderSDK.dll` next to the executable

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
