#include <cubos/core/settings.hpp>

#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/transform/position.hpp>
#include <cubos/engine/transform/rotation.hpp>
#include <cubos/engine/transform/scale.hpp>

using namespace cubos;

static void applyTransform(
    core::ecs::Query<engine::LocalToWorld&, const engine::Position*, const engine::Rotation*, const engine::Scale*>
        query)
{
    for (auto [entity, localToWorld, position, rotation, scale] : query)
    {
        localToWorld.mat = glm::mat4(1.0F);
        if (position != nullptr)
        {
            localToWorld.mat = glm::translate(localToWorld.mat, position->vec);
        }
        if (rotation != nullptr)
        {
            localToWorld.mat *= glm::toMat4(rotation->quat);
        }
        if (scale != nullptr)
        {
            localToWorld.mat = glm::scale(localToWorld.mat, glm::vec3(scale->factor));
        }
    }
}

void cubos::engine::transformPlugin(Cubos& cubos)
{
    cubos.addComponent<Position>();
    cubos.addComponent<Rotation>();
    cubos.addComponent<Scale>();
    cubos.addComponent<LocalToWorld>();

    cubos.system(applyTransform).tagged("cubos.transform.update");
}
