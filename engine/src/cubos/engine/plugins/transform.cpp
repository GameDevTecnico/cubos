#include <cubos/core/settings.hpp>

#include <cubos/engine/ecs/local_to_world.hpp>
#include <cubos/engine/ecs/position.hpp>
#include <cubos/engine/ecs/rotation.hpp>
#include <cubos/engine/ecs/scale.hpp>
#include <cubos/engine/plugins/transform.hpp>

using namespace cubos;

static void applyTransform(core::ecs::Query<engine::ecs::LocalToWorld&, const engine::ecs::Position*,
                                            const engine::ecs::Rotation*, const engine::ecs::Scale*>
                               query)
{
    for (auto [entity, localToWorld, position, rotation, scale] : query)
    {
        localToWorld.mat = glm::mat4(1.0f);
        if (position != nullptr)
            localToWorld.mat = glm::translate(localToWorld.mat, position->vec);
        if (rotation != nullptr)
            localToWorld.mat *= glm::toMat4(rotation->quat);
        if (scale != nullptr)
            localToWorld.mat = glm::scale(localToWorld.mat, glm::vec3(scale->factor));
    }
}

void cubos::engine::plugins::transformPlugin(Cubos& cubos)
{
    cubos.addComponent<ecs::Position>();
    cubos.addComponent<ecs::Rotation>();
    cubos.addComponent<ecs::Scale>();
    cubos.addComponent<ecs::LocalToWorld>();

    cubos.system(applyTransform).tagged("cubos.transform.update");
}
