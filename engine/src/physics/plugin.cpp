#include <glm/glm.hpp>

#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::physicsApplyForcesTag);
CUBOS_DEFINE_TAG(cubos::engine::physicsSimulationApplyImpulsesTag);
CUBOS_DEFINE_TAG(cubos::engine::physicsSimulationSubstepsIntegrateTag);
CUBOS_DEFINE_TAG(cubos::engine::physicsSimulationSubstepsCorrectPositionTag);
CUBOS_DEFINE_TAG(cubos::engine::physicsSimulationSubstepsUpdateVelocityTag);
CUBOS_DEFINE_TAG(cubos::engine::physicsSimulationClearForcesTag);

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

CUBOS_REFLECT_IMPL(Damping)
{
    return core::ecs::TypeBuilder<Damping>("cubos::engine::Damping").build();
}

void cubos::engine::physicsPlugin(Cubos& cubos)
{
    cubos.depends(fixedStepPlugin);
    cubos.depends(transformPlugin);

    cubos.resource<Damping>();

    cubos.component<Velocity>();
    cubos.component<Force>();
    cubos.component<Impulse>();
    cubos.component<Mass>();
    cubos.component<AccumulatedCorrection>();
    cubos.component<PreviousPosition>();
    cubos.component<PhysicsBundle>();

    cubos.observer("unpack PhysicsBundle's")
        .onAdd<PhysicsBundle>()
        .call([](Commands cmds, Query<Entity, const PhysicsBundle&> query) {
            for (auto [ent, bundle] : query)
            {
                cmds.remove<PhysicsBundle>(ent);

                auto force = Force{};
                force.add(bundle.force);

                auto impulse = Impulse{};
                impulse.add(bundle.impulse);

                cmds.add(ent, PreviousPosition{});
                cmds.add(ent, Mass{.mass = bundle.mass, .inverseMass = 1.0F / bundle.mass});
                cmds.add(ent, Velocity{.vec = bundle.velocity});
                cmds.add(ent, force);
                cmds.add(ent, impulse);
                cmds.add(ent, AccumulatedCorrection{});
            }
        });

    cubos.tag(physicsApplyForcesTag);
    cubos.tag(physicsSimulationApplyImpulsesTag);
    cubos.tag(physicsSimulationSubstepsIntegrateTag).after(physicsApplyForcesTag);
    cubos.tag(physicsSimulationSubstepsCorrectPositionTag);
    cubos.tag(physicsSimulationSubstepsUpdateVelocityTag);
    cubos.tag(physicsSimulationClearForcesTag);

    cubos.system("apply impulses")
        .tagged(physicsSimulationApplyImpulsesTag)
        .after(physicsApplyForcesTag)
        .before(physicsSimulationSubstepsIntegrateTag)
        .tagged(fixedStepTag)
        .call([](Query<Velocity&, const Impulse&, const Mass&> query) {
            for (auto [velocity, impulse, mass] : query)
            {
                velocity.vec += impulse.vec() * mass.inverseMass;
            }
        });

    cubos.system("integrate position")
        .tagged(physicsSimulationSubstepsIntegrateTag)
        .tagged(fixedSubstepTag)
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

    cubos.system("update velocities")
        .tagged(physicsSimulationSubstepsUpdateVelocityTag)
        .after(physicsSimulationSubstepsCorrectPositionTag)
        .tagged(fixedSubstepTag)
        .call([](Query<const Position&, const PreviousPosition&, Velocity&> query, const FixedDeltaTime& fixedDeltaTime,
                 const Substeps& substeps) {
            float subDeltaTime = fixedDeltaTime.value / (float)substeps.value;

            for (auto [position, prevPosition, velocity] : query)
            {
                // consider changing this to accumulated correction due to floating point accuracy for objects far from
                // origin
                velocity.vec = (position.vec - prevPosition.vec) / subDeltaTime;
            }
        });

    cubos.system("clear forces")
        .tagged(physicsSimulationClearForcesTag)
        .after(physicsSimulationSubstepsUpdateVelocityTag)
        .tagged(fixedStepTag)
        .call([](Query<Force&> query) {
            for (auto [force] : query)
            {
                force.clear();
            }
        });

    cubos.system("clear impulses")
        .tagged(physicsSimulationClearForcesTag)
        .after(physicsSimulationSubstepsUpdateVelocityTag)
        .tagged(fixedStepTag)
        .call([](Query<Impulse&> query) {
            for (auto [impulse] : query)
            {
                impulse.clear();
            }
        });
}
