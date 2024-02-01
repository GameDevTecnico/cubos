#include <cubos/engine/transform/plugin.hpp>

void cubos::engine::transformPlugin(Cubos& cubos)
{
    cubos.addComponent<Position>();
    cubos.addComponent<Rotation>();
    cubos.addComponent<Scale>();
    cubos.addComponent<LocalToWorld>();
    cubos.addComponent<LocalToParent>();
    cubos.addRelation<ChildOf>();

    cubos.system("add LocalToWorld to entities with Position")
        .tagged("cubos.transform.missing.local_to_world")
        .without<LocalToWorld>()
        .with<Position>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [e] : query)
            {
                cmds.add(e, LocalToWorld{});
            }
        });

    cubos.system("add LocalToWorld to entities with Rotation")
        .tagged("cubos.transform.missing.local_to_world")
        .without<LocalToWorld>()
        .with<Rotation>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [e] : query)
            {
                cmds.add(e, LocalToWorld{});
            }
        });

    cubos.system("add LocalToWorld to entities with Scale")
        .tagged("cubos.transform.missing.local_to_world")
        .without<LocalToWorld>()
        .with<Scale>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [e] : query)
            {
                cmds.add(e, LocalToWorld{});
            }
        });

    cubos.tag("cubos.transform.missing.local_to_world").before("cubos.transform.missing");

    cubos.system("add Position to entities with LocalToWorld")
        .tagged("cubos.transform.missing")
        .without<Position>()
        .with<LocalToWorld>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [e] : query)
            {
                cmds.add(e, Position{});
            }
        });

    cubos.system("add Rotation to entities with LocalToWorld")
        .tagged("cubos.transform.missing")
        .without<Rotation>()
        .with<LocalToWorld>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [e] : query)
            {
                cmds.add(e, Rotation{});
            }
        });

    cubos.system("add Scale to entities with LocalToWorld")
        .tagged("cubos.transform.missing")
        .without<Scale>()
        .with<LocalToWorld>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [e] : query)
            {
                cmds.add(e, Scale{});
            }
        });

    cubos.tag("cubos.transform.missing").before("cubos.transform.update");

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
