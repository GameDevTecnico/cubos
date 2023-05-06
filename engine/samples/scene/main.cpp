#include <imgui.h>

#include <cubos/core/data/debug_serializer.hpp>
#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/std_archive.hpp>
#include <cubos/core/gl/light.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/memory/stream.hpp>

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/data/asset_manager.hpp>
#include <cubos/engine/data/palette.hpp>
#include <cubos/engine/data/scene.hpp>
#include <cubos/engine/ecs/camera.hpp>
#include <cubos/engine/ecs/grid.hpp>
#include <cubos/engine/ecs/local_to_world.hpp>
#include <cubos/engine/ecs/position.hpp>
#include <cubos/engine/ecs/rotation.hpp>
#include <cubos/engine/ecs/scale.hpp>
#include <cubos/engine/env_settings/plugin.hpp>
#include <cubos/engine/file_settings/plugin.hpp>
#include <cubos/engine/gl/frame.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/tools/settings_inspector/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

#include "num.hpp"
#include "parent.hpp"

using namespace cubos;
using namespace engine;
using namespace core::ecs;
using namespace core::data;
using namespace core::memory;
using namespace cubos::engine;

void setup(data::AssetManager& assetManager)
{
    CUBOS_INFO("SettingUp");
    FileSystem::mount("/assets/", std::make_shared<STDArchive>(SAMPLE_ASSETS_FOLDER, true, true));

    assetManager.registerType<data::Palette>();
    assetManager.registerType<data::Scene>();
    assetManager.importMeta(FileSystem::find("/assets/"));

    // Load and spawn the scene.
    auto scene = assetManager.load<data::Scene>("scenes/main");
}

void printStuff(World& world)
{
    for (auto entity : world)
    {
        auto name = std::to_string(entity.index);
        auto ser = DebugSerializer(Stream::stdOut);
        auto pkg = world.pack(entity, [](core::data::Serializer& ser, const core::data::Handle& handle,
                                         const char* name) { ser.write(handle.getId(), name); });
        ser.write(pkg, name.c_str());
        Stream::stdOut.put('\n');
    }
}

void createScene(World& world, data::AssetManager& assetManager, gl::Renderer& renderer,
                 plugins::ActiveCamera& activeCamera)
{
    ecs::Camera camera;
    camera.fovY = 60.0f;
    camera.zNear = 0.1f;
    camera.zFar = 1000.0f;
    activeCamera.entity =
        world.create(ecs::LocalToWorld{}, ecs::Camera{camera}, ecs::Position{{0.0f, 40.0f, -70.0f}},
                     ecs::Rotation{glm::quatLookAt(glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 1.0f, 0.0f})});

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

    world.create(ecs::Grid{floor, {-128.0f, -1.0f, -128.0f}}, ecs::LocalToWorld{}, ecs::Position{}, ecs::Scale{4.0f});
}

static void turnOnLight(gl::Frame& frame)
{
    frame.ambient({0.1f, 0.1f, 0.1f});

    // Add a directional light to the frame.
    glm::quat directionalLightRotation = glm::quat(glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0));
    frame.light(core::gl::DirectionalLight(directionalLightRotation, glm::vec3(1), 1.0f));
}

static void imguiExampleWindow()
{
    ImGui::Begin("hi !!!");
    ImGui::End();
}

int main(int argc, char** argv)
{
    // Initialize the asset manager.
    Cubos cubos(argc, argv);

    cubos.addPlugin(plugins::envSettingsPlugin);
    cubos.addPlugin(plugins::fileSettingsPlugin);
    cubos.addPlugin(plugins::rendererPlugin);
    cubos.addPlugin(plugins::imguiPlugin);
    cubos.addPlugin(plugins::tools::settingsInspectorPlugin);

    cubos.addResource<data::AssetManager>();

    cubos.addComponent<Num>();
    cubos.addComponent<Parent>();

    cubos.startupSystem(setup).tagged("setup").afterTag("cubos.renderer.init");
    cubos.startupSystem(createScene).tagged("create").afterTag("setup");
    cubos.startupSystem(printStuff).afterTag("create");

    cubos.system(imguiExampleWindow).tagged("cubos.imgui");
    cubos.system(turnOnLight).beforeTag("cubos.renderer.draw");

    cubos.run();
}
