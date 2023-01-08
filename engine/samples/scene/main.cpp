#include <cubos/core/log.hpp>
#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/std_archive.hpp>
#include <cubos/core/data/debug_serializer.hpp>

#include <cubos/core/memory/stream.hpp>

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/data/asset_manager.hpp>
#include <cubos/engine/data/scene.hpp>

#include <cubos/engine/plugins/window.hpp>
#include <cubos/engine/plugins/env_settings.hpp>
#include <cubos/engine/plugins/file_settings.hpp>
#include <cubos/engine/plugins/imgui.hpp>

#include <components/num.hpp>
#include <components/parent.hpp>

#include <imgui.h>

using namespace cubos;
using namespace engine;
using namespace core::ecs;
using namespace core::data;
using namespace core::memory;
using namespace cubos::engine;

void setup(Commands cmds, data::AssetManager& assetManager)
{
    FileSystem::mount("/assets/", std::make_shared<STDArchive>(SAMPLE_ASSETS_FOLDER, true, true));

    assetManager.registerType<data::Scene>();
    assetManager.importMeta(FileSystem::find("/assets/"));

    // Load and spawn the scene.
    auto scene = assetManager.load<data::Scene>("scenes/main");
    auto root = cmds.create().entity();
    cmds.spawn(scene->blueprint).add("main", Parent{root});
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
    cubos.addPlugin(plugins::windowPlugin);
    cubos.addPlugin(cubos::engine::plugins::fileSettingsPlugin);

    // an example of how the imgui plugin can be used to render your own stuff :)
    cubos.addPlugin(plugins::imguiPlugin);

    cubos.addResource<data::AssetManager>().addComponent<Num>().addComponent<Parent>();

    cubos.tag("ImGuiExampleWindow").afterTag("BeginImGuiFrame");

    cubos.startupSystem(setup).tagged("Setup");

    cubos.startupSystem(printStuff).tagged("End");

    cubos.system(imguiExampleWindow).tagged("ImGuiExampleWindow");

    cubos.run();
}
