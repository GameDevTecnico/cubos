#include "fixed_substep/plugin.hpp"

#include <glm/glm.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/collisions/collider_bundle.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/voxel.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/rigid_body_bundle.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::physicsApplyForcesTag);

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(PhysicsBundle)
{
    return cubos::core::ecs::TypeBuilder<PhysicsBundle>("cubos::engine::PhysicsBundle")
        .withField("mass", &PhysicsBundle::mass)
        .withField("velocity", &PhysicsBundle::velocity)
        .withField("angularVelocity", &PhysicsBundle::angularVelocity)
        .withField("force", &PhysicsBundle::force)
        .withField("torque", &PhysicsBundle::torque)
        .withField("impulse", &PhysicsBundle::impulse)
        .withField("angularImpulse", &PhysicsBundle::angularImpulse)
        .withField("material", &PhysicsBundle::material)
        .withField("centerOfMass", &PhysicsBundle::centerOfMass)
        .withField("inertiaTensor", &PhysicsBundle::inertiaTensor)
        .build();
}

CUBOS_REFLECT_IMPL(Damping)
{
    return core::ecs::TypeBuilder<Damping>("cubos::engine::Damping").build();
}

CUBOS_REFLECT_IMPL(SolverConstants)
{
    return core::ecs::TypeBuilder<SolverConstants>("cubos::engine::SolverConstants")
        .withField("minInvMass", &SolverConstants::minInvMass)
        .withField("minInvInertia", &SolverConstants::minInvInertia)
        .withField("maxBias", &SolverConstants::maxBias)
        .withField("minContactHertz", &SolverConstants::minContactHertz)
        .withField("minKNormal", &SolverConstants::minKNormal)
        .withField("minKFriction", &SolverConstants::minKFriction)
        .withField("minTangentLenSq", &SolverConstants::minTangentLenSq)
        .withField("minRestitution", &SolverConstants::minRestitution)
        .withField("minNormalSpeed", &SolverConstants::minNormalSpeed)
        .withField("minNormalImpulse", &SolverConstants::minNormalImpulse)
        .build();
}

CUBOS_DEFINE_TAG(cubos::engine::physicsPrepareTag);

void cubos::engine::physicsPlugin(Cubos& cubos)
{
    cubos.depends(fixedStepPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(assetsPlugin);

    cubos.plugin(physicsFixedSubstepPlugin);

    cubos.tag(physicsPrepareTag).after(collisionsTag).tagged(fixedStepTag);

    cubos.resource<Damping>();
    cubos.resource<SolverConstants>();

    cubos.component<Velocity>();
    cubos.component<AngularVelocity>();
    cubos.component<Force>();
    cubos.component<Torque>();
    cubos.component<Impulse>();
    cubos.component<AngularImpulse>();
    cubos.component<Mass>();
    cubos.component<Inertia>();
    cubos.component<CenterOfMass>();
    cubos.component<AccumulatedCorrection>();
    cubos.component<PhysicsMaterial>();
    cubos.component<PhysicsBundle>();
    cubos.component<RigidBodyBundle>();

    cubos.observer("unpack PhysicsBundle's")
        .onAdd<PhysicsBundle>()
        .call([](Commands cmds, Query<Entity, const PhysicsBundle&> query) {
            for (auto [ent, bundle] : query)
            {
                cmds.remove<PhysicsBundle>(ent);

                auto force = Force{};
                force.add(bundle.force);

                auto torque = Torque{};
                torque.add(bundle.torque);

                auto impulse = Impulse{};
                impulse.add(bundle.impulse);

                auto angularImpulse = AngularImpulse{};
                angularImpulse.add(bundle.angularImpulse);

                cmds.add(ent, Mass{.mass = bundle.mass, .inverseMass = 1.0F / bundle.mass});
                cmds.add(ent, CenterOfMass{.vec = bundle.centerOfMass});
                if (bundle.inertiaTensor != glm::mat3(0.0F))
                {
                    cmds.add(ent, Inertia{.inertia = bundle.inertiaTensor,
                                          .inverseInertia = glm::inverse(bundle.inertiaTensor),
                                          .autoUpdate = false});
                }
                else
                {
                    cmds.add(
                        ent,
                        Inertia{.inertia = glm::mat3(0.0F), .inverseInertia = glm::mat3(0.0F), .autoUpdate = true});
                }
                cmds.add(ent, Velocity{.vec = bundle.velocity});
                cmds.add(ent, AngularVelocity{.vec = bundle.angularVelocity});
                cmds.add(ent, force);
                cmds.add(ent, torque);
                cmds.add(ent, impulse);
                cmds.add(ent, angularImpulse);
                cmds.add(ent, AccumulatedCorrection{});
                cmds.add(ent, bundle.material);
            }
        });

    cubos.observer("unpack RigidBodyBundle's")
        .onAdd<RigidBodyBundle>()
        .call([](Commands cmds, Query<Entity, const RigidBodyBundle&> query) {
            for (auto [ent, bundle] : query)
            {
                cmds.remove<RigidBodyBundle>(ent);

                auto force = Force{};
                force.add(bundle.force);

                auto torque = Torque{};
                torque.add(bundle.torque);

                auto impulse = Impulse{};
                impulse.add(bundle.impulse);

                auto angularImpulse = AngularImpulse{};
                angularImpulse.add(bundle.angularImpulse);

                cmds.add(ent, Mass{.mass = bundle.mass, .inverseMass = 1.0F / bundle.mass});
                cmds.add(ent, CenterOfMass{.vec = bundle.centerOfMass});
                if (!bundle.autoInertiaTensor)
                {
                    cmds.add(ent, Inertia{.inertia = bundle.inertiaTensor,
                                          .inverseInertia = glm::inverse(bundle.inertiaTensor),
                                          .autoUpdate = false});
                }
                else
                {
                    cmds.add(
                        ent,
                        Inertia{.inertia = glm::mat3(0.0F), .inverseInertia = glm::mat3(0.0F), .autoUpdate = true});
                }
                cmds.add(ent, Velocity{.vec = bundle.velocity});
                cmds.add(ent, AngularVelocity{.vec = bundle.angularVelocity});
                cmds.add(ent, force);
                cmds.add(ent, torque);
                cmds.add(ent, impulse);
                cmds.add(ent, angularImpulse);
                cmds.add(ent, AccumulatedCorrection{});
                cmds.add(ent, bundle.material);

                cmds.add(ent, ColliderBundle{.isArea = false,
                                             .isStatic = false,
                                             .isActive = bundle.isActive,
                                             .layers = bundle.layers,
                                             .mask = bundle.mask});
            }
        });

    // Should this be here?
    // Updates center of mass and inertia for a body with a box shape collider based on changes in the shape and mass
    // TODO: Update CenterOfMass when we change the offset of the collider
    cubos.system("update mass properties - box collider")
        .tagged(physicsPrepareTag)
        .call([](Query<Mass&, BoxCollisionShape&, Inertia&, CenterOfMass&> query) {
            for (auto [mass, shape, inertia, centerOfMass] : query)
            {
                if (!inertia.autoUpdate)
                {
                    continue;
                }

                if (!mass.changed && !shape.changed)
                {
                    continue;
                }

                mass.changed = false;
                shape.changed = false;

                // Object has infinite mass
                if (mass.inverseMass == 0.0F)
                {
                    inertia.inertia = glm::mat3(0.0F);
                    inertia.inverseInertia = glm::mat3(0.0F);
                    continue;
                }

                // TODO: change this to adapt to the offset with the collider transform
                centerOfMass.vec = glm::vec3(0.0F, 0.0F, 0.0F);

                // Recalculate inertia tensor
                glm::mat3 inertiaTensor = boxShapeInertiaTensor(mass.mass, shape.box.halfSize * 2.0F);
                inertia.setFromTensor(inertiaTensor);
            }
        });

    // Should this be here?
    // Updates center of mass and inertia for a body with a voxel shape collider based on changes in the shape and mass
    // TODO: Update CenterOfMass when we change the offset of the collider
    cubos.system("update mass properties - voxel collider")
        .tagged(physicsPrepareTag)
        .call([](Query<Mass&, VoxelCollisionShape&, Inertia&, CenterOfMass&> query) {
            for (auto [mass, shape, inertia, centerOfMass] : query)
            {
                if (!inertia.autoUpdate)
                {
                    continue;
                }

                if (!mass.changed && !shape.changed)
                {
                    continue;
                }

                mass.changed = false;
                shape.changed = false;

                // Object has infinite mass
                if (mass.inverseMass == 0.0F)
                {
                    inertia.inertia = glm::mat3(0.0F);
                    inertia.inverseInertia = glm::mat3(0.0F);
                    continue;
                }

                // TODO: change this to adapt to the offset with the collider transform
                centerOfMass.vec = glm::vec3(0.0F, 0.0F, 0.0F);

                // Recalculate inertia tensor
                glm::mat3 inertiaTensor = voxelShapeInertiaTensor(mass.mass, shape);
                inertia.setFromTensor(inertiaTensor);
            }
        });

    cubos.tag(physicsApplyForcesTag).tagged(fixedStepTag);
}
