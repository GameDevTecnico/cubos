#include <cubos/core/settings.hpp>

#include <cubos/engine/transform/plugin.hpp>

using cubos::core::ecs::Query;
using namespace cubos::engine;

static void applyTransform(Query<LocalToWorld&, const Position*, const Rotation*, const Scale*> query)
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
