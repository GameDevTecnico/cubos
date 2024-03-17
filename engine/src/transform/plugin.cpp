#include <cubos/engine/transform/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::transformMissingTag);
CUBOS_DEFINE_TAG(cubos::engine::transformMissingLocalTag);
CUBOS_DEFINE_TAG(cubos::engine::transformUpdateRelativeTag);
CUBOS_DEFINE_TAG(cubos::engine::transformUpdateTag);
CUBOS_DEFINE_TAG(cubos::engine::transformUpdatePropagateTag);

using cubos::core::ecs::Traversal;
using cubos::engine::Commands;
using cubos::engine::Entity;
using cubos::engine::Query;

template <typename T>
void addComponent(Commands cmds, Query<Entity> query)
{
    for (auto [e] : query)
    {
        cmds.add(e, T{});
    }
}

void cubos::engine::transformPlugin(Cubos& cubos)
{
    cubos.addComponent<Position>();
    cubos.addComponent<Rotation>();
    cubos.addComponent<Scale>();
    cubos.addComponent<LocalToWorld>();
    cubos.addComponent<LocalToParent>();
    cubos.addRelation<ChildOf>();

    cubos.observer("add LocalToWorld when Position is added")
        .onAdd<Position>()
        .without<LocalToWorld>()
        .call(addComponent<LocalToWorld>);

    cubos.observer("add LocalToWorld when Rotation is added")
        .onAdd<Rotation>()
        .without<LocalToWorld>()
        .call(addComponent<LocalToWorld>);

    cubos.observer("add LocalToWorld when Scale is added")
        .onAdd<Scale>()
        .without<LocalToWorld>()
        .call(addComponent<LocalToWorld>);

    cubos.observer("add Position when LocalToWorld is added")
        .onAdd<LocalToWorld>()
        .without<Position>()
        .call(addComponent<Position>);

    cubos.observer("add Rotation when LocalToWorld is added")
        .onAdd<LocalToWorld>()
        .without<Rotation>()
        .call(addComponent<Rotation>);

    cubos.observer("add Scale when LocalToWorld is added")
        .onAdd<LocalToWorld>()
        .without<Scale>()
        .call(addComponent<Scale>);

    cubos.observer("add LocalToParent to entities with LocalToWorld")
        .onAdd<LocalToWorld>()
        .without<LocalToParent>()
        .call(addComponent<LocalToParent>);

    cubos.system("update LocalToParent's")
        .tagged(transformUpdateTag)
        .before(transformUpdatePropagateTag)
        .call([](Query<LocalToParent&, const Position&, const Rotation&, const Scale&> query) {
            for (auto [localToParent, position, rotation, scale] : query)
            {
                localToParent.mat =
                    glm::scale(glm::translate(glm::mat4(1.0F), position.vec) * glm::toMat4(rotation.quat),
                               glm::vec3(scale.factor));
            }
        });

    cubos.system("update LocalToWorld's of children")
        .tagged(transformUpdateTag)
        .tagged(transformUpdatePropagateTag)
        .with<LocalToWorld>()
        .with<LocalToParent>()
        .related<ChildOf>(Traversal::Down)
        .with<LocalToWorld>()
        .call([](Query<LocalToWorld&, const LocalToParent&, const LocalToWorld&> children,
                 Query<LocalToWorld&, const LocalToParent&> all) {
            for (auto [localToWorld, localToParent] : all)
            {
                localToWorld.mat = localToParent.mat;
            }

            for (auto [localToWorld, localToParent, parentLocalToWorld] : children)
            {
                localToWorld.mat = parentLocalToWorld.mat * localToParent.mat;
            }
        });
}
