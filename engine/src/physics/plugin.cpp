#include <glm/glm.hpp>

#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/solver.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "physics_accumulator.hpp"

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(AccumulatedCorrection)
{
    return cubos::core::ecs::TypeBuilder<AccumulatedCorrection>("cubos::engine::AccumulatedCorrection")
        .withField("vec", &AccumulatedCorrection::vec)
        .build();
}

CUBOS_REFLECT_IMPL(Mass)
{
    return cubos::core::ecs::TypeBuilder<Mass>("cubos::engine::Mass")
        .withField("mass", &Mass::mass)
        .withField("inverseMass", &Mass::inverseMass)
        .build();
}

CUBOS_REFLECT_IMPL(Velocity)
{
    return cubos::core::ecs::TypeBuilder<Velocity>("cubos::engine::Velocity")
        .withField("velocity", &Velocity::vec)
        .build();
}

CUBOS_REFLECT_IMPL(Force)
{
    return cubos::core::ecs::TypeBuilder<Force>("cubos::engine::Force").withField("mForce", &Force::mForce).build();
}

CUBOS_REFLECT_IMPL(Impulse)
{
    return cubos::core::ecs::TypeBuilder<Impulse>("cubos::engine::Impulse")
        .withField("mImpulse", &Impulse::mImpulse)
        .build();
}

CUBOS_REFLECT_IMPL(PreviousPosition)
{
    return cubos::core::ecs::TypeBuilder<PreviousPosition>("cubos::engine::PreviousPosition")
        .withField("vec", &PreviousPosition::vec)
        .build();
}

CUBOS_REFLECT_IMPL(PhysicsBundle)
{
    return cubos::core::ecs::TypeBuilder<PhysicsBundle>("cubos::engine::PhysicsBundle")
        .withField("mass", &PhysicsBundle::mass)
        .withField("velocity", &PhysicsBundle::velocity)
        .withField("force", &PhysicsBundle::force)
        .withField("impulse", &PhysicsBundle::impulse)
        .build();
}

static bool simulatePhysicsStep(PhysicsAccumulator& accumulator, const FixedDeltaTime& fixedDeltaTime)
{
    return accumulator.value >= fixedDeltaTime.value;
}

void cubos::engine::physicsPlugin(Cubos& cubos)
{
    cubos.addResource<FixedDeltaTime>();
    cubos.addResource<Substeps>();
    cubos.addResource<PhysicsAccumulator>();
    cubos.addResource<Damping>();

    cubos.addComponent<Velocity>();
    cubos.addComponent<Force>();
    cubos.addComponent<Impulse>();
    cubos.addComponent<Mass>();
    cubos.addComponent<AccumulatedCorrection>();
    cubos.addComponent<PreviousPosition>();
    cubos.addComponent<PhysicsBundle>();

    cubos.addPlugin(collisionsPlugin);
    cubos.addPlugin(gravityPlugin);
    cubos.addPlugin(solverPlugin);

    // add components to entities created with PhysicsBundle
    cubos.system("unpack PhysicsBundle's")
        .tagged("cubos.physics.unpack_bundle")
        .call([](Commands cmds, Query<Entity, const PhysicsBundle&> query) {
            for (auto [ent, bundle] : query)
            {
                cmds.add(ent, PreviousPosition{});
                cmds.add(ent, Mass{.mass = bundle.mass, .inverseMass = 1.0F / bundle.mass});
                cmds.add(ent, Velocity{.vec = bundle.velocity});

                auto force = Force{};
                force.add(bundle.force);
                cmds.add(ent, force);

                auto impulse = Impulse{};
                impulse.add(bundle.impulse);
                cmds.add(ent, impulse);

                cmds.add(ent, AccumulatedCorrection{});
                cmds.remove<PhysicsBundle>(ent);
            }
        });

    // executed every frame
    cubos.system("increase fixed-step accumulator")
        .tagged("cubos.physics.simulation.prepare")
        .call(
            [](PhysicsAccumulator& accumulator, const DeltaTime& deltaTime) { accumulator.value += deltaTime.value; });

    cubos.tag("cubos.physics.apply_forces")
        .after("cubos.physics.simulation.prepare")
        .before("cubos.physics.simulation.substeps.integrate")
        .runIf(simulatePhysicsStep);

    cubos.system("apply impulses")
        .tagged("cubos.physics.simulation.apply_impulses")
        .after("cubos.physics.apply_forces")
        .before("cubos.physics.simulation.substeps.integrate")
        .onlyIf(simulatePhysicsStep)
        .call([](Query<Velocity&, const Impulse&, const Mass&> query) {
            for (auto [velocity, impulse, mass] : query)
            {
                velocity.vec += impulse.vec() * mass.inverseMass;
            }
        });

    cubos.system("integrate position")
        .tagged("cubos.physics.simulation.substeps.integrate")
        .after("cubos.physics.simulation.prepare")
        .onlyIf(simulatePhysicsStep)
        .call([](Query<Position&, PreviousPosition&, Velocity&, const Force&, const Mass&> query,
                 const Damping& damping, const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps) {
            float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;

            for (auto [position, prevPosition, velocity, force, mass] : query)
            {
                prevPosition.vec = position.vec;

                if (mass.inverseMass <= 0.0F)
                {
                    continue;
                }

                // Apply damping
                velocity.vec *= glm::pow(damping.value, subDeltaTime);

                // Apply external forces
                glm::vec3 deltaLinearVelocity = force.vec() * mass.inverseMass * subDeltaTime;

                velocity.vec += deltaLinearVelocity;
                position.vec += velocity.vec * subDeltaTime;
            }
        });

    cubos.system("apply corrections to positions")
        .tagged("cubos.physics.simulation.substeps.correct_position")
        .after("cubos.physics.simulation.substeps.integrate")
        .onlyIf(simulatePhysicsStep)
        .call([](Query<Position&, AccumulatedCorrection&, Mass&> query) {
            for (auto [position, correction, mass] : query)
            {
                if (mass.inverseMass <= 0.0F)
                {
                    continue;
                }
                position.vec += correction.vec; // lagrange * correction * inverseMass
                correction.vec = glm::vec3(0, 0, 0);
            }
        });

    cubos.system("update velocities")
        .tagged("cubos.physics.simulation.substeps.update_velocity")
        .after("cubos.physics.simulation.substeps.correct_position")
        .onlyIf(simulatePhysicsStep)
        .call([](Query<const Position&, const PreviousPosition&, Velocity&> query, const FixedDeltaTime& fixedDeltaTime,
                 const Substeps& substeps) {
            float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;

            for (auto [position, prevPosition, velocity] : query)
            {
                velocity.vec = (position.vec - prevPosition.vec) / subDeltaTime;
            }
        });

    cubos.system("clear forces")
        .tagged("cubos.physics.simulation.clear_forces")
        .after("cubos.physics.simulation.substeps.update_velocity")
        .onlyIf(simulatePhysicsStep)
        .call([](Query<Force&> query) {
            for (auto [force] : query)
            {
                force.clear();
            }
        });

    cubos.system("clear impulses")
        .tagged("cubos.physics.simulation.clear_forces")
        .after("cubos.physics.simulation.substeps.update_velocity")
        .onlyIf(simulatePhysicsStep)
        .call([](Query<Impulse&> query) {
            for (auto [impulse] : query)
            {
                impulse.clear();
            }
        });

    cubos.system("decrease fixed-step accumulator")
        .tagged("cubos.physics.simulation.decrease_accumulator")
        .after("cubos.physics.simulation.clear_forces")
        .onlyIf(simulatePhysicsStep)
        .call([](PhysicsAccumulator& accumulator, const FixedDeltaTime& fixedDeltaTime) {
            accumulator.value -= fixedDeltaTime.value;
        });
}
