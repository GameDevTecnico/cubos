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

    cubos.resource<MagiConfigIntegration>();

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

    cubos.system("apply angular impulses")
        .tagged(physicsApplyImpulsesTag)
        .after(physicsApplyForcesTag)
        .before(physicsIntegrateVelocityTag)
        .tagged(fixedStepTag)
        .call([](Query<AngularVelocity&, const AngularImpulse&, const Inertia&, const Rotation&> query) {
            for (auto [angVelocity, angImpulse, inertia, rotation] : query)
            {
                angVelocity.vec += inertia.rotatedInverseInertia(rotation.quat) * angImpulse.vec();
            }
        });

    cubos.system("integrate velocity")
        .tagged(physicsIntegrateVelocityTag)
        .call([](Query<Velocity&, AngularVelocity&, const Force&, const Torque&, const Mass&, const Inertia&,
                       const Rotation&>
                     query,
                 const Damping& damping, const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps,
                 MagiConfigIntegration& magiConfig) {
            float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;

            for (auto [velocity, angVelocity, force, torque, mass, inertia, rotation] : query)
            {
                // Linear velocity
                if (magiConfig.cmpInvMass(mass.inverseMass))
                {
                    continue;
                }

                // Apply damping
                velocity.vec *= glm::pow(damping.value, subDeltaTime);

                // Apply external forces
                glm::vec3 deltaLinearVelocity = force.vec() * mass.inverseMass * subDeltaTime;

                velocity.vec += deltaLinearVelocity;

                // Angular velocity
                if (magiConfig.cmpInvInertia(inertia.inverseInertia))
                {
                    continue;
                }

                // Apply damping
                angVelocity.vec *= glm::pow(damping.value, subDeltaTime);

                // Rotate inertia tensor
                glm::mat3 rotatedInverseInertia = inertia.rotatedInverseInertia(rotation.quat);

                // Apply external torque
                glm::vec3 deltaAngularVelocity = rotatedInverseInertia * (force.torque() + torque.vec()) * subDeltaTime;

                angVelocity.vec += deltaAngularVelocity;
            }
        });

    cubos.system("integrate delta position")
        .tagged(physicsIntegratePositionTag)
        .call([](Query<AccumulatedCorrection&, Rotation&, const Velocity&, const AngularVelocity&, const Mass&,
                       const Inertia&>
                     query,
                 const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps, MagiConfigIntegration& magiConfig) {
            float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;

            for (auto [correction, rotation, velocity, angVelocity, mass, inertia] : query)
            {
                // Position
                if (magiConfig.cmpInvMass(mass.inverseMass))
                {
                    continue;
                }

                correction.position += velocity.vec * subDeltaTime;

                // Rotation
                if (magiConfig.cmpInvInertia(inertia.inverseInertia))
                {
                    continue;
                }

                rotation.quat =
                    glm::normalize(rotation.quat +
                                   (glm::quat(glm::vec4(angVelocity.vec * subDeltaTime * 0.5F, 0.0F)) * rotation.quat));
            }
        });

    cubos.system("finalize position")
        .tagged(physicsFinalizePositionTag)
        .call([](Query<Position&, AccumulatedCorrection&, const Mass&> query, MagiConfigIntegration& magiConfig) {
            for (auto [position, correction, mass] : query)
            {
                if (magiConfig.cmpInvMass(mass.inverseMass))
                {
                    continue;
                }

                position.vec += correction.position;
                correction.position = glm::vec3(0.0F);
            }
        });

    cubos.system("clear forces").tagged(physicsClearForcesTag).call([](Query<Force&> query) {
        for (auto [force] : query)
        {
            force.clear();
        }
    });

    cubos.system("clear torque").tagged(physicsClearForcesTag).call([](Query<Torque&> query) {
        for (auto [torque] : query)
        {
            torque.clear();
        }
    });

    cubos.system("clear impulses").tagged(physicsClearForcesTag).call([](Query<Impulse&> query) {
        for (auto [impulse] : query)
        {
            impulse.clear();
        }
    });

    cubos.system("clear angular impulses").tagged(physicsClearForcesTag).call([](Query<AngularImpulse&> query) {
        for (auto [angImpulse] : query)
        {
            angImpulse.clear();
        }
    });
}
