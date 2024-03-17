#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <tesseratos/collider_gizmos/plugin.hpp>
#include <tesseratos/entity_selector/plugin.hpp>
#include <tesseratos/toolbox/plugin.hpp>

using cubos::engine::BoxCollisionShape;
using cubos::engine::Commands;
using cubos::engine::Cubos;
using cubos::engine::Entity;
using cubos::engine::Gizmos;
using cubos::engine::LocalToWorld;
using cubos::engine::Position;
using cubos::engine::Query;

void tesseratos::colliderGizmosPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::gizmosPlugin);
    cubos.addPlugin(cubos::engine::collisionsPlugin);
    cubos.addPlugin(toolboxPlugin);

    cubos.system("draw box collision shape gizmos")
        .after(cubos::engine::TransformUpdateTag)
        .before(cubos::engine::GizmosDrawTag)
        .call([](Toolbox& toolbox, Gizmos& gizmos, const EntitySelector& selector,
                 Query<Entity, const LocalToWorld&, const BoxCollisionShape&> boxes) {
            bool showAll = toolbox.isOpen("All Collider Gizmos");

            gizmos.color({1.0F, 0.0F, 0.0F});

            for (auto [ent, localToWorld, shape] : boxes)
            {
                if (showAll || ent == selector.selection)
                {
                    auto size = shape.box.halfSize * 2.0F;
                    auto transform = glm::scale(localToWorld.mat, size);
                    gizmos.drawWireBox("collider-gizmos", transform);
                }
            }
        });
}
