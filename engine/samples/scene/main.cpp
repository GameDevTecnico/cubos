#include <cubos/core/data/ser/debug.hpp>
#include <cubos/core/memory/stream.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>

#include "components.hpp"

using cubos::core::ecs::World;

using namespace cubos::engine;

/// [Component Refl]
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(Num)
{
    return cubos::core::ecs::TypeBuilder<Num>("Num").withField("value", &Num::value).build();
}

CUBOS_REFLECT_IMPL(OwnedBy)
{
    return cubos::core::ecs::TypeBuilder<OwnedBy>("OwnedBy").tree().build();
}

CUBOS_REFLECT_IMPL(DistanceTo)
{
    return cubos::core::ecs::TypeBuilder<DistanceTo>("DistanceTo")
        .symmetric()
        .withField("value", &DistanceTo::value)
        .build();
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
static void printStuff(Query<Entity, const Num&> numQuery, Query<const OwnedBy&, Entity> ownedByQuery,
                       Query<const DistanceTo&, Entity> distanceToQuery)
{
    using cubos::core::data::DebugSerializer;
    using cubos::core::memory::Stream;

    DebugSerializer ser{Stream::stdOut};

    for (auto [entity, num] : numQuery)
    {
        Stream::stdOut.print("Entity ");
        ser.write(entity);
        Stream::stdOut.printf(":\n- Num = {}\n", num.value);

        for (auto [distanceTo, what] : distanceToQuery.pin(0, entity))
        {
            Stream::stdOut.print("- DistanceTo(");
            ser.write(what);
            Stream::stdOut.printf(") = {}\n", distanceTo.value);
        }

        for (auto [ownedBy, owner] : ownedByQuery.pin(0, entity))
        {
            Stream::stdOut.print("- OwnedBy(");
            ser.write(owner);
            Stream::stdOut.print(")\n");
        }
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
    cubos.addRelation<OwnedBy>();
    cubos.addRelation<DistanceTo>();

    cubos.startupSystem(settings).tagged("cubos.settings");

    /// [Adding the system]
    cubos.startupSystem(spawnScene).tagged("spawn").tagged("cubos.assets");
    /// [Adding the system]

    cubos.startupSystem(printStuff).after("spawn");
    cubos.run();
}
