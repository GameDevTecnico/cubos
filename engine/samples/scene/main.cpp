#include <cubos/core/log.hpp>
#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/std_archive.hpp>
#include <cubos/core/data/debug_serializer.hpp>

#include <cubos/core/memory/stream.hpp>

#include <cubos/core/gl/light.hpp>

#include <cubos/engine/gl/frame.hpp>

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/data/asset_manager.hpp>
#include <cubos/engine/data/scene.hpp>
#include <cubos/engine/data/palette.hpp>

#include <cubos/engine/plugins/transform.hpp>
#include <cubos/engine/plugins/window.hpp>
#include <cubos/engine/plugins/renderer.hpp>
#include <cubos/engine/plugins/env_settings.hpp>

#include <components/cubos/position.hpp>
#include <components/cubos/rotation.hpp>
#include <components/cubos/scale.hpp>
#include <components/cubos/local_to_world.hpp>
#include <components/cubos/grid.hpp>
#include <components/cubos/camera.hpp>

#include <components/num.hpp>
#include <components/parent.hpp>

using namespace cubos;
using namespace engine;
using namespace core::ecs;
using namespace core::data;
using namespace core::memory;

void setup(Commands& cmds, data::AssetManager& assetManager)
{
    FileSystem::mount("/assets/", std::make_shared<STDArchive>(SAMPLE_ASSETS_FOLDER, true, true));

    assetManager.registerType<data::Scene>();
    assetManager.importMeta(FileSystem::find("/assets/"));

    // Load and spawn the scene.
    auto scene = assetManager.load<data::Scene>("scenes/main");
    auto root = cmds.create().entity();
    cmds.spawn(scene->blueprint).add("main", Parent{root});
}

void printStuff(Debug debug)
{
    for (auto [entity, pkg] : debug)
    {
        auto name = std::to_string(entity.index);
        auto ser = DebugSerializer(Stream::stdOut);
        ser.write(pkg, name.c_str());
        Stream::stdOut.put('\n');
    }
}

void createScene(core::ecs::Commands& commands, data::AssetManager& assetManager, gl::Renderer& renderer,
                 plugins::ActiveCamera& activeCamera)
{
    ecs::Camera camera;
    camera.fovY = 60.0f;
    camera.zNear = 0.1f;
    camera.zFar = 1000.0f;
    activeCamera.entity =
        commands
            .create(ecs::LocalToWorld{}, ecs::Camera{camera}, ecs::Position{{0.0f, 40.0f, -70.0f}},
                    ecs::Rotation{glm::quatLookAt(glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 1.0f, 0.0f})})
            .entity();

    assetManager.registerType<data::Palette>();
    auto paletteAsset = assetManager.load<data::Palette>("palette");
    auto palette = paletteAsset->palette;

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

    commands.create(ecs::Grid{floor, {-128.0f, -1.0f, -128.0f}}, ecs::LocalToWorld{}, ecs::Position{},
                    ecs::Scale{4.0f});
} //

static void TurnOnLight(gl::Frame& frame)
{
    frame.ambient({0.1f, 0.1f, 0.1f});

    // Add a directional light to the frame.
    glm::quat directionalLightRotation = glm::quat(glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0));
    frame.light(core::gl::DirectionalLight(directionalLightRotation, glm::vec3(1), 1.0f));
}

int main(int argc, char** argv)
{
    // Initialize the asset manager.
    Cubos(argc, argv)
        .addResource<data::AssetManager>()
        .addComponent<Num>()
        .addComponent<Parent>()

        .addPlugin(cubos::engine::plugins::envSettingsPlugin)
        .addPlugin(cubos::engine::plugins::windowPlugin)
        .addPlugin(cubos::engine::plugins::transformPlugin)
        .addPlugin(cubos::engine::plugins::rendererPlugin)

        .addStartupSystem(setup, "Setup")
        .addStartupSystem(createScene, "CreateScene")
        .addStartupSystem(printStuff, "End")

        .addSystem(TurnOnLight, "SetLight")
        .putStageBefore("SetLight", "Draw")
        .run();
}
