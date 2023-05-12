#include <components/car.hpp>
#include <components/cubos/camera.hpp>
#include <components/cubos/grid.hpp>
#include <components/cubos/local_to_world.hpp>
#include <components/cubos/position.hpp>
#include <components/cubos/rotation.hpp>
#include <components/cubos/scale.hpp>

#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/std_archive.hpp>
#include <cubos/core/ecs/commands.hpp>
#include <cubos/core/gl/grid.hpp>

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/data/asset_manager.hpp>
#include <cubos/engine/data/grid.hpp>
#include <cubos/engine/data/palette.hpp>
#include <cubos/engine/gl/deferred/renderer.hpp>
#include <cubos/engine/gl/frame.hpp>
#include <cubos/engine/gl/renderer.hpp>
#include <cubos/engine/plugins/file_settings.hpp>
#include <cubos/engine/plugins/renderer.hpp>
#include <cubos/engine/plugins/window.hpp>

using namespace cubos;
using namespace engine;
using namespace core::ecs;
using namespace core::data;
using namespace cubos::engine;

void setup(Commands cmds, data::AssetManager& assetManager, gl::Renderer& renderer, plugins::ActiveCamera& activeCamera)
{
    FileSystem::mount("/assets/", std::make_shared<STDArchive>(SAMPLE_ASSETS_FOLDER, true, true));

    // Import meta.
    assetManager.registerType<data::Palette>();
    assetManager.importMeta(FileSystem::find("/assets/"));

    // Get the palette.
    auto paletteAsset = assetManager.load<data::Palette>("palette");
    auto palette = paletteAsset->palette;

    // Generate the floor's grid.
    auto black = palette.add({{0.1f, 0.1f, 0.1f, 1.0f}});
    auto white = palette.add({{0.9f, 0.9f, 0.9f, 1.0f}});
    auto floorGrid = core::gl::Grid({256, 1, 256});
    for (int x = 0; x < 256; ++x)
    {
        for (int z = 0; z < 256; ++z)
        {
            floorGrid.set({x, 0, z}, (x + z) % 2 == 0 ? black : white);
        }
    }
    renderer->setPalette(palette);

    auto floorRendererGrid = renderer->upload(floorGrid);
    auto floor = assetManager.store<data::Grid>("floor", data::Usage::Static,
                                                data::Grid{
                                                    .grid = std::move(floorGrid),
                                                    .rendererGrid = floorRendererGrid,
                                                });

    // Spawn the floor.
    cmds.create(ecs::Grid{floor, {-128.0f, -1.0f, -128.0f}}, ecs::LocalToWorld{}, ecs::Position{}, ecs::Scale{4.0f});

    // Load car grid.
    assetManager.registerType<data::Grid>(renderer.get());

    // Spawn the camera.
    auto camera =
        cmds.create(ecs::Camera{60.0f, 0.1f, 1000.0f}, ecs::LocalToWorld{}, ecs::Position{{0.0f, 40.0f, -70.0f}},
                    ecs::Rotation{glm::quatLookAt(glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 1.0f, 0.0f})});

    activeCamera.entity = camera.entity();
}

void spawnSystem(Commands cmds, data::AssetManager& assetManager, const DeltaTime& deltaTime)
{
    static float timer = 0.0f;
    static int x = -2;
    static int y = -1;
    timer += deltaTime.value;

    if (timer >= 2)
    {
        auto car = assetManager.load<data::Grid>("car");
        cmds.create(Car{},
                    ecs::Grid{car, {-float(car->grid.getSize().x) / 2.0f, 0.0f, -float(car->grid.getSize().z) / 2.0f}},
                    ecs::LocalToWorld{}, ecs::Position{{56.0f * x, 0.0f, 56.0f * y}}, ecs::Rotation{});
        timer = 0;
        x++;
        if (x == 4)
        {
            y++;
            x = -2;
        }
    }
}

static void turnOnLight(gl::Frame& frame)
{
    frame.ambient({0.1f, 0.1f, 0.1f});

    // Add a directional light to the frame.
    glm::quat directionalLightRotation = glm::quat(glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0));
    frame.light(core::gl::DirectionalLight(directionalLightRotation, glm::vec3(1), 1.0f));
}

void moveSystem(core::ecs::Query<Car&, ecs::Position&, ecs::Rotation&> query, const DeltaTime& deltaTime)
{
    for (auto [entity, car, position, rotation] : query)
    {
        car.vel = rotation.quat * glm::vec3(2.0f, 0.0f, 2.0f);
        car.angVel = 0.3f;

        position.vec += deltaTime.value * car.vel;
        rotation.quat = glm::angleAxis(deltaTime.value * car.angVel, glm::vec3(0.0f, 1.0f, 0.0f)) * rotation.quat;
    }
}

int main(int argc, char** argv)
{
    auto cubos = Cubos(argc, argv);

    cubos.addPlugin(plugins::windowPlugin);
    cubos.addPlugin(plugins::rendererPlugin);
    cubos.addPlugin(plugins::fileSettingsPlugin);

    cubos.addResource<data::AssetManager>()
        .addComponent<ecs::LocalToWorld>()
        .addComponent<ecs::Position>()
        .addComponent<ecs::Rotation>()
        .addComponent<ecs::Scale>()
        .addComponent<ecs::Grid>()
        .addComponent<Car>();

    cubos.startupSystem(setup).tagged("setup").afterTag("cubos.renderer.init");

    cubos.tag("SpawnSystem").beforeTag("MoveSystem");
    cubos.system(spawnSystem).tagged("SpawnSystem");
    cubos.system(moveSystem).tagged("MoveSystem");

    cubos.system(turnOnLight).beforeTag("cubos.renderer.draw");

    cubos.run();
}