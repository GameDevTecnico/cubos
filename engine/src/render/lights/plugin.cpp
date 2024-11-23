#include <cubos/engine/render/active/active.hpp>
#include <cubos/engine/render/active/plugin.hpp>
#include <cubos/engine/render/lights/directional.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/lights/plugin.hpp>
#include <cubos/engine/render/lights/point.hpp>
#include <cubos/engine/render/lights/spot.hpp>

void cubos::engine::lightsPlugin(Cubos& cubos)
{
    cubos.resource<RenderEnvironment>();
    cubos.depends(activePlugin);

    cubos.component<DirectionalLight>();
    cubos.component<PointLight>();
    cubos.component<SpotLight>();

    cubos.observer("add active component on add DirectionalLight")
        .onAdd<DirectionalLight>()
        .without<Active>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [ent] : query)
            {
                cmds.add(ent, Active{});
            }
        });

    cubos.observer("add active component on add PointLight")
        .onAdd<PointLight>()
        .without<Active>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [ent] : query)
            {
                cmds.add(ent, Active{});
            }
        });

    cubos.observer("add active component on add SpotLight")
        .onAdd<SpotLight>()
        .without<Active>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [ent] : query)
            {
                cmds.add(ent, Active{});
            }
        });
}
