#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/tools/collider_gizmos/plugin.hpp>
#include <cubos/engine/tools/selection/plugin.hpp>
#include <cubos/engine/tools/toolbox/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

void cubos::engine::colliderGizmosPlugin(Cubos& cubos)
{
    cubos.depends(gizmosPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(toolboxPlugin);
    cubos.depends(selectionPlugin);

    cubos.system("draw box collision shape gizmos")
        .after(transformUpdateTag)
        .before(gizmosDrawTag)
        .call([](Toolbox& toolbox, Gizmos& gizmos, const Selection& selection,
                 Query<Entity, const LocalToWorld&, const Collider&, const BoxCollisionShape&> boxes) {
            bool showAll = toolbox.isOpen("All Collider Gizmos");

            gizmos.color({1.0F, 0.0F, 0.0F});

            for (auto [ent, localToWorld, collider, shape] : boxes)
            {
                if (showAll || ent == selection.entity)
                {
                    auto size = shape.box.halfSize * 2.0F;
                    auto transform = glm::scale(localToWorld.mat, size);
                    gizmos.drawWireBox("collider-gizmos", transform);
                }
            }
        });
}
