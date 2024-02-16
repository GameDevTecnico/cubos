#include <glm/glm.hpp>

#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/physics/components/accumulated_correction.hpp>
#include <cubos/engine/physics/solver/solver.hpp>

using namespace cubos::engine;

void cubos::engine::solverPlugin(Cubos& cubos)
{
    cubos.system("solve collision")
        .tagged("cubos.physics.solve")
        .after("cubos.physics.simulation.substeps.integrate")
        .after("cubos.collisions.narrow")
        .before("cubos.physics.simulation.substeps.correct_position")
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
