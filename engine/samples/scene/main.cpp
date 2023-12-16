#include <cubos/core/data/ser/debug.hpp>
#include <cubos/core/memory/stream.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>

#include "components.hpp"

using cubos::core::ecs::World;

using namespace cubos::engine;

/// [Component Refl]
#include <cubos/core/ecs/component/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(Num)
{
    return cubos::core::ecs::ComponentTypeBuilder<Num>("Num").withField("value", &Num::value).build();
}

CUBOS_REFLECT_IMPL(Parent)
{
    return cubos::core::ecs::ComponentTypeBuilder<Parent>("Parent").withField("entity", &Parent::entity).build();
}
/// [Component Refl]

/// [Setting the asset]
static const Asset<Scene> SceneAsset = AnyAsset("f0d86ba8-5f34-440f-a180-d9d12c8e8b91");
/// [Setting the asset]

static void settings(Settings& settings)
{
    settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

/// [Spawning the scene]
static void spawnScene(Commands commands, const Assets& assets)
{
    auto sceneRead = assets.read(SceneAsset);
    commands.spawn(sceneRead->blueprint);
}
/// [Spawning the scene]

/// [Displaying the scene]
static void printStuff(const World& world, Query<Entity> query)
{
    using cubos::core::data::DebugSerializer;
    using cubos::core::memory::Stream;

    DebugSerializer ser{Stream::stdOut};

    for (auto [entity] : query)
    {
        Stream::stdOut.printf("Entity ");
        ser.write(entity);
        Stream::stdOut.put('\n');
        for (auto [type, value] : world.components(entity))
        {
            Stream::stdOut.printf("- {}: ", type->name());
            ser.write(*type, value);
            Stream::stdOut.put('\n');
        }
        Stream::stdOut.put('\n');
    }
}
/// [Displaying the scene]

int main(int argc, char** argv)
{
    Cubos cubos(argc, argv);

    /// [Adding the plugin]
    cubos.addPlugin(scenePlugin);
    /// [Adding the plugin]

    cubos.addComponent<Num>();
    cubos.addComponent<Parent>();

    cubos.startupSystem(settings).tagged("cubos.settings");

    /// [Adding the system]
    cubos.startupSystem(spawnScene).tagged("spawn").tagged("cubos.assets");
    /// [Adding the system]

    cubos.startupSystem(printStuff).after("spawn");
    cubos.run();
}
