#include <cubos/engine/transform/plugin.hpp>

using cubos::core::ecs::Traversal;

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

    cubos.system("add LocalToParent to entities with LocalToWord and ChildOf other entity with LocalToWorld")
        .tagged("cubos.transform.missing")
        .entity()
        .without<LocalToParent>()
        .with<LocalToWorld>()
        .related<ChildOf>()
        .with<LocalToWorld>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [e] : query)
            {
                cmds.add(e, LocalToParent{});
            }
        });

    cubos.tag("cubos.transform.missing").before("cubos.transform.update.relative");

    cubos.system("update relative transform matrix")
        .tagged("cubos.transform.update.relative")
        .before("cubos.transform.update")
        .call([](Query<LocalToWorld&, Opt<LocalToParent&>, const Position&, const Rotation&, const Scale&> query) {
            for (auto [localToWorld, localToParent, position, rotation, scale] : query)
            {
                auto mat = glm::scale(glm::translate(localToWorld.mat, position.vec) * glm::toMat4(rotation.quat),
                                      glm::vec3(scale.factor));

                if (localToParent)
                {
                    localToParent->mat = mat;
                }
                else
                {
                    // No parent, so transform is relative to the world
                    localToWorld.mat = mat;
                }
            }
        });

    cubos.system("update LocalToWorlds of children")
        .tagged("cubos.transform.update")
        .with<LocalToWorld>()
        .with<LocalToParent>()
        .related<ChildOf>(Traversal::Down)
        .with<LocalToWorld>()
        .call([](Query<LocalToWorld&, const LocalToParent&, const LocalToWorld&> query) {
            for (auto [localToWorld, localToParent, parentLocalToWorld] : query)
            {
                localToWorld.mat = parentLocalToWorld.mat * localToParent.mat;
            }
        });
}
