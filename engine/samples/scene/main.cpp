#include <cubos/core/data/ser/debug.hpp>
#include <cubos/core/memory/stream.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>

#include "components.hpp"

using cubos::core::ecs::World;

using namespace cubos::engine;

namespace cubos::engine
{
    extern Tag spawnTag;
}
CUBOS_DEFINE_TAG(cubos::engine::spawnTag);

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

int main(int argc, char** argv)
{
    Cubos cubos(argc, argv);

    /// [Adding the plugin]
    cubos.addPlugin(scenePlugin);
    /// [Adding the plugin]

    cubos.addComponent<Num>();
    cubos.addRelation<OwnedBy>();
    cubos.addRelation<DistanceTo>();

    cubos.startupSystem("configure Assets").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    });

    /// [Spawning the scene]
    cubos.startupSystem("spawn the scene")
        .tagged(spawnTag)
        .tagged(assetsTag)
        .call([](Commands commands, const Assets& assets) {
            auto sceneRead = assets.read(SceneAsset);
            commands.spawn(sceneRead->blueprint);
        });
    /// [Spawning the scene]

    /// [Printing the scene]
    cubos.startupSystem("print the scene")
        .after(spawnTag)
        .call([](Query<Entity, const Num&> numQuery, Query<const OwnedBy&, Entity> ownedByQuery,
                 Query<const DistanceTo&, Entity> distanceToQuery) {
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
        });
    /// [Printing the scene]

    cubos.run();
}
