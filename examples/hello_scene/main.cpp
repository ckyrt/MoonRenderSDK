#include <MoonRender/MoonRender.h>

int main()
{
    MoonRender::Runtime runtime;
    runtime.Initialize({ "MoonRender Hello Scene", 1280, 720 });

    MoonRender::World world = runtime.CreateWorld();
    MoonRender::MaterialHandle groundMaterial = world.CreateMaterial({
        MoonRender::MaterialPreset::ConcreteFloor,
        MoonRender::MappingMode::Triplanar
    });

    world.CreateSky({ MoonRender::SkyType::Procedural, 1.0f });
    world.SetWeather({ MoonRender::WeatherType::Clear, 12.0f });
    world.CreateGround({ { 100.0f, 100.0f }, groundMaterial });
    world.CreateDirectionalLight({});

    runtime.Tick();
    runtime.Render(world);
    runtime.Shutdown();
    return 0;
}
