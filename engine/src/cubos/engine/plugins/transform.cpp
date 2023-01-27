#include <cubos/engine/plugins/transform.hpp>

#include <cubos/core/settings.hpp>

#include <components/cubos/position.hpp>
#include <components/cubos/rotation.hpp>
#include <components/cubos/scale.hpp>
#include <components/cubos/local_to_world.hpp>

void applyTransform(cubos::core::ecs::Query<cubos::engine::ecs::LocalToWorld&, const cubos::engine::ecs::Position*,
                                            const cubos::engine::ecs::Rotation*, const cubos::engine::ecs::Scale*>
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
    cubos.addComponent<ecs::Position>()
        .addComponent<ecs::Rotation>()
        .addComponent<ecs::Scale>()
        .addComponent<ecs::LocalToWorld>();

    cubos.system(applyTransform).tagged("Transform");
}
