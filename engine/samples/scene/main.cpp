#include <cubos/core/data/ser/debug.hpp>
#include <cubos/core/ecs/name.hpp>
#include <cubos/core/memory/stream.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "components.hpp"

using cubos::core::ecs::Name;
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
    return cubos::core::ecs::TypeBuilder<Num>("Num").wrap(&Num::value);
}

CUBOS_REFLECT_IMPL(OwnedBy)
{
    return cubos::core::ecs::TypeBuilder<OwnedBy>("OwnedBy").tree().build();
}

CUBOS_REFLECT_IMPL(DistanceTo)
{
    return cubos::core::ecs::TypeBuilder<DistanceTo>("DistanceTo").symmetric().wrap(&DistanceTo::value);
}
/// [Component Refl]

/// [Setting the asset]
static const Asset<Scene> SceneAsset = AnyAsset("f0d86ba8-5f34-440f-a180-d9d12c8e8b91");
/// [Setting the asset]

int main(int argc, char** argv)
{
    Cubos cubos(argc, argv);

    /// [Adding the plugin]
    cubos.plugin(settingsPlugin);
    cubos.plugin(assetsPlugin);
    cubos.plugin(transformPlugin);
    cubos.plugin(scenePlugin);
    /// [Adding the plugin]

    cubos.component<Num>();
    cubos.relation<OwnedBy>();
    cubos.relation<DistanceTo>();

    cubos.startupTag(spawnTag);

    cubos.startupSystem("configure Assets").before(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
    });

    /// [Spawning the scene]
    cubos.startupSystem("spawn the scene")
        .tagged(spawnTag)
        .tagged(assetsTag)
        .call([](Commands commands, const Assets& assets) { commands.spawn(*assets.read(SceneAsset)).named("scene"); });
    /// [Spawning the scene]

    /// [Printing the scene]
    cubos.startupSystem("print the scene")
        .after(spawnTag)
        .call([](Query<Entity, const Name&> nameQuery, Query<const Num&> numQuery,
                 Query<const OwnedBy&, Entity> ownedByQuery, Query<const DistanceTo&, Entity> distanceToQuery,
                 Query<const ChildOf&, Entity> childOfQuery) {
            using cubos::core::data::DebugSerializer;
            using cubos::core::memory::Stream;

            DebugSerializer ser{Stream::stdOut};

            for (auto [entity, name] : nameQuery)
            {
                Stream::stdOut.print("Entity ");
                ser.write(entity);
                Stream::stdOut.printf(":\n- Name = {}\n", name.value);

                for (auto [num] : numQuery.pin(0, entity))
                {
                    Stream::stdOut.printf("- Num = {}\n", num.value);
                }

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

                for (auto [childOf, parent] : childOfQuery.pin(0, entity))
                {
                    Stream::stdOut.print("- ChildOf(");
                    ser.write(parent);
                    Stream::stdOut.print(")\n");
                }
            }
        });
    /// [Printing the scene]

    cubos.run();
}
