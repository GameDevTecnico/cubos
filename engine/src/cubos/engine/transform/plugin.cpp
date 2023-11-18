#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

static void autoLocalToWorld(Commands cmds,
                             Query<OptRead<LocalToWorld>, OptRead<Position>, OptRead<Rotation>, OptRead<Scale>> query)
{
    for (auto [entity, localToWorld, position, rotation, scale] : query)
    {
        if (!localToWorld && (position || rotation || scale))
        {
            cmds.add(entity, LocalToWorld{});
        }
    }
}

static void applyTransform(Query<Write<LocalToWorld>, OptRead<Position>, OptRead<Rotation>, OptRead<Scale>> query)
{
    for (auto [entity, localToWorld, position, rotation, scale] : query)
    {
        localToWorld->mat = glm::mat4(1.0F);
        if (position)
        {
            localToWorld->mat = glm::translate(localToWorld->mat, position->vec);
        }

        if (rotation)
        {
            localToWorld->mat *= glm::toMat4(rotation->quat);
        }

        if (scale)
        {
            localToWorld->mat = glm::scale(localToWorld->mat, glm::vec3(scale->factor));
        }
    }
}

void cubos::engine::transformPlugin(Cubos& cubos)
{
    cubos.addComponent<Position>();
    cubos.addComponent<Rotation>();
    cubos.addComponent<Scale>();
    cubos.addComponent<LocalToWorld>();

    cubos.system(autoLocalToWorld).before("cubos.transform.update");
    cubos.system(applyTransform).tagged("cubos.transform.update");
}
