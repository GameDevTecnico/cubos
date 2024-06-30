#include "plugin.hpp"

#include <glm/glm.hpp>

#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/physics/components/accumulated_correction.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "../../fixed_substep/plugin.hpp"

using namespace cubos::engine;

CUBOS_DEFINE_TAG(cubos::engine::physicsApplyImpulsesTag);
CUBOS_DEFINE_TAG(cubos::engine::physicsClearForcesTag);

void cubos::engine::physicsIntegrationPlugin(Cubos& cubos)
{
    cubos.depends(fixedStepPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(physicsPlugin);
    cubos.depends(physicsSolverPlugin);

    cubos.tag(physicsApplyImpulsesTag);
    cubos.tag(physicsClearForcesTag).after(physicsFinalizePositionTag).tagged(fixedStepTag);

    cubos.system("apply impulses")
        .tagged(physicsApplyImpulsesTag)
        .after(physicsApplyForcesTag)
        .before(physicsIntegrateVelocityTag)
        .tagged(fixedStepTag)
        .call([](Query<Velocity&, const Impulse&, const Mass&> query) {
            for (auto [velocity, impulse, mass] : query)
            {
                velocity.vec += impulse.vec() * mass.inverseMass;
            }
        });

    cubos.system("integrate velocity")
        .tagged(physicsIntegrateVelocityTag)
        .call([](Query<Velocity&, const Force&, const Mass&> query, const Damping& damping,
                 const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps) {
            float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;

            for (auto [velocity, force, mass] : query)
            {
                if (mass.inverseMass <= 0.0F)
                {
                    continue;
                }

                // Apply damping
                velocity.vec *= glm::pow(damping.value, subDeltaTime);

                // Apply external forces
                glm::vec3 deltaLinearVelocity = force.vec() * mass.inverseMass * subDeltaTime;

                velocity.vec += deltaLinearVelocity;
            }
        });

    cubos.system("integrate delta position")
        .tagged(physicsIntegratePositionTag)
        .call([](Query<AccumulatedCorrection&, Velocity&, const Mass&> query, const FixedDeltaTime& fixedDeltaTime,
                 const Substeps& substeps) {
            float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;

            for (auto [correction, velocity, mass] : query)
            {
                if (mass.inverseMass <= 0.0F)
                {
                    continue;
                }

                correction.position += velocity.vec * subDeltaTime;
            }
        });

    cubos.system("finalize position")
        .tagged(physicsFinalizePositionTag)
        .call([](Query<Position&, AccumulatedCorrection&, const Mass&> query) {
            for (auto [position, correction, mass] : query)
            {
                if (mass.inverseMass <= 0.0F)
                {
                    continue;
                }
                position.vec += correction.position;
                correction.position = glm::vec3(0.0F);
                correction.impulse = 0.0F;
            }
        });

    cubos.system("clear forces").tagged(physicsClearForcesTag).call([](Query<Force&> query) {
        for (auto [force] : query)
        {
            force.clear();
        }
    });

    cubos.system("clear impulses").tagged(physicsClearForcesTag).call([](Query<Impulse&> query) {
        for (auto [impulse] : query)
        {
            impulse.clear();
        }
    });
}
