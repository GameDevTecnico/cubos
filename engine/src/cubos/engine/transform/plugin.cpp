#include <cubos/engine/transform/plugin.hpp>

void cubos::engine::transformPlugin(Cubos& cubos)
{
    cubos.addComponent<Position>();
    cubos.addComponent<Rotation>();
    cubos.addComponent<Scale>();
    cubos.addComponent<LocalToWorld>();
    cubos.addComponent<LocalToParent>();
    cubos.addRelation<ChildOf>();

    cubos.system("add LocalToWorld where needed")
        .before("cubos.transform.update")
        .call([](Commands cmds,
                 Query<Entity, Opt<const LocalToWorld&>, Opt<const Position&>, Opt<const Rotation&>, Opt<const Scale&>>
                     query) {
            for (auto [entity, localToWorld, position, rotation, scale] : query)
            {
                if (!localToWorld && (position || rotation || scale))
                {
                    cmds.add(entity, LocalToWorld{});
                }
            }
        });

    cubos.system("update LocalToWorld")
        .tagged("cubos.transform.update")
        .call([](Query<Entity, LocalToWorld&, Opt<const Position&>, Opt<const Rotation&>, Opt<const Scale&>> query) {
            for (auto [entity, localToWorld, position, rotation, scale] : query)
            {
                localToWorld.mat = glm::mat4(1.0F);
                if (position)
                {
                    localToWorld.mat = glm::translate(localToWorld.mat, position->vec);
                }

                if (rotation)
                {
                    localToWorld.mat *= glm::toMat4(rotation->quat);
                }

                if (scale)
                {
                    localToWorld.mat = glm::scale(localToWorld.mat, glm::vec3(scale->factor));
                }
            }
        });
}
