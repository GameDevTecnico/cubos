#include <glm/glm.hpp>

#include <cubos/engine/physics/plugin.hpp>
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

CUBOS_REFLECT_IMPL(PhysicsMass)
{
    return cubos::core::ecs::TypeBuilder<PhysicsMass>("cubos::engine::PhysicsMass")
        .withField("mass", &PhysicsMass::mass)
        .withField("inverseMass", &PhysicsMass::inverseMass)
        .build();
}

CUBOS_REFLECT_IMPL(Velocity)
{
    return cubos::core::ecs::TypeBuilder<Velocity>("cubos::engine::Velocity")
        .withField("velocity", &Velocity::velocity)
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

static bool simulatePhysicsStep(PhysicsAccumulator& accumulator, const FixedDeltaTime& fixedDeltaTime)
{
    return accumulator.value >= fixedDeltaTime.value;
}

void cubos::engine::physicsPlugin(Cubos& cubos)
{
    cubos.addPlugin(gravityPlugin);

    cubos.addResource<FixedDeltaTime>();
    cubos.addResource<Substeps>();
    cubos.addResource<PhysicsAccumulator>();
    cubos.addResource<Damping>();

    cubos.addComponent<Velocity>();
    cubos.addComponent<Force>();
    cubos.addComponent<Impulse>();
    cubos.addComponent<PhysicsMass>();
    cubos.addComponent<AccumulatedCorrection>();
    cubos.addComponent<PreviousPosition>();

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
        .call([](Query<Velocity&, const Impulse&, const PhysicsMass&> query) {
            for (auto [velocity, impulse, mass] : query)
            {
                velocity.velocity += impulse.getImpulse() * mass.inverseMass;
            }
        });

    cubos.system("integrate position")
        .tagged("cubos.physics.simulation.substeps.integrate")
        .after("cubos.physics.simulation.prepare")
        .onlyIf(simulatePhysicsStep)
        .call([](Query<Position&, PreviousPosition&, Velocity&, const Force&, const PhysicsMass&> query,
                 const Damping& damping, const FixedDeltaTime& fixedDeltaTime, const Substeps& substeps) {
            float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;

            for (auto [position, prevPosition, velocity, force, mass] : query)
            {
                prevPosition.vec = position.vec;

                if (mass.inverseMass <= 0.0F)
                {
                    return;
                }

                // Apply damping
                velocity.velocity *= glm::pow(damping.value, subDeltaTime);

                // Apply external forces
                glm::vec3 deltaLinearVelocity = force.getForce() * mass.inverseMass * subDeltaTime;

                velocity.velocity += deltaLinearVelocity;
                position.vec += velocity.velocity * subDeltaTime;
            }
        });

    cubos.system("apply corrections to positions")
        .tagged("cubos.physics.simulation.substeps.correct_position")
        .after("cubos.physics.simulation.substeps.integrate")
        .onlyIf(simulatePhysicsStep)
        .call([](Query<Position&, AccumulatedCorrection&> query) {
            for (auto [position, correction] : query)
            {
                position.vec += correction.vec;
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
                velocity.velocity = (position.vec - prevPosition.vec) / subDeltaTime;
            }
        });

    cubos.system("clear forces")
        .tagged("cubos.physics.simulation.clear_forces")
        .after("cubos.physics.simulation.substeps.update_velocity")
        .onlyIf(simulatePhysicsStep)
        .call([](Query<Force&> query) {
            for (auto [force] : query)
            {
                force.clearForce();
            }
        });

    cubos.system("clear impulses")
        .tagged("cubos.physics.simulation.clear_forces")
        .after("cubos.physics.simulation.substeps.update_velocity")
        .onlyIf(simulatePhysicsStep)
        .call([](Query<Impulse&> query) {
            for (auto [impulse] : query)
            {
                impulse.clearImpulse();
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
