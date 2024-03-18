#include <glm/glm.hpp>

#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/physics/components/accumulated_correction.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/solver.hpp>

#include "../collisions/narrow_phase/plugin.hpp"

CUBOS_DEFINE_TAG(cubos::engine::physicsSolveTag);

using namespace cubos::engine;

void cubos::engine::solverPlugin(Cubos& cubos)
{
    cubos.depends(collisionsPlugin);
    cubos.depends(physicsPlugin);

    cubos.tag(physicsSolveTag);

    cubos.system("solve collision")
        .tagged(physicsSolveTag)
        .after(physicsSimulationSubstepsIntegrateTag)
        .after(collisionsNarrowTag)
        .before(physicsSimulationSubstepsCorrectPositionTag)
        .call([](Query<Entity, AccumulatedCorrection&, const CollidingWith&, Entity, AccumulatedCorrection&> query) {
            for (auto [ent1, correction1, colliding, ent2, correction2] : query)
            {
                auto penetration = colliding.penetration;
                if (ent1 == colliding.entity)
                {
                    correction1.vec -= colliding.normal * (penetration / 1.0F);
                    correction2.vec += colliding.normal * (penetration / 1.0F);
                }
                else
                {
                    correction1.vec += colliding.normal * (penetration / 1.0F);
                    correction2.vec -= colliding.normal * (penetration / 1.0F);
                }
            }
        });
}
