#include "plugin.hpp"

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "../entity_selector/plugin.hpp"
#include "../toolbox/plugin.hpp"

using namespace cubos::engine;

void tesseratos::colliderGizmosPlugin(Cubos& cubos)
{
    cubos.depends(cubos::engine::gizmosPlugin);
    cubos.depends(cubos::engine::collisionsPlugin);
    cubos.depends(cubos::engine::transformPlugin);

    cubos.depends(toolboxPlugin);
    cubos.depends(entitySelectorPlugin);

    cubos.system("draw box collision shape gizmos")
        .after(cubos::engine::transformUpdateTag)
        .before(cubos::engine::gizmosDrawTag)
        .call([](Toolbox& toolbox, Gizmos& gizmos, const EntitySelector& selector,
                 Query<Entity, const LocalToWorld&, const Collider&, const BoxCollisionShape&> boxes) {
            bool showAll = toolbox.isOpen("All Collider Gizmos");

            gizmos.color({1.0F, 0.0F, 0.0F});

            for (auto [ent, localToWorld, collider, shape] : boxes)
            {
                if (showAll || ent == selector.selection)
                {
                    auto size = shape.box.halfSize * 2.0F;
                    auto transform = glm::scale(localToWorld.mat * collider.transform, size);
                    gizmos.drawWireBox("collider-gizmos", transform);
                }
            }
        });
}
