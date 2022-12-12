#include <cubos/engine/plugins/transform.hpp>
#include <cubos/engine/ecs/transform_system.hpp>

#include <cubos/core/settings.hpp>

#include <components/cubos/position.hpp>
#include <components/cubos/rotation.hpp>
#include <components/cubos/scale.hpp>
#include <components/cubos/local_to_world.hpp>

void cubos::engine::plugins::transformPlugin(Cubos& cubos)
{
    cubos.addComponent<ecs::Position>()
        .addComponent<ecs::Rotation>()
        .addComponent<ecs::Scale>()
        .addComponent<ecs::LocalToWorld>()

        .addSystem(ecs::transformSystem, "PrepareDraw");
}
