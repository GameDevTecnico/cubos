#include "fixed_substep/plugin.hpp"

#include <glm/glm.hpp>

#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::physicsApplyForcesTag);

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(AccumulatedCorrection)
{
    return cubos::core::ecs::TypeBuilder<AccumulatedCorrection>("cubos::engine::AccumulatedCorrection")
        .withField("position", &AccumulatedCorrection::position)
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

CUBOS_REFLECT_EXTERNAL_IMPL(PhysicsMaterial::MixProperty)
{
    return cubos::core::reflection::Type::create("cubos::engine::PhysicsMaterial::MixProperty")
        .with(cubos::core::reflection::EnumTrait{}
                  .withVariant<PhysicsMaterial::MixProperty::Maximum>("Maximum")
                  .withVariant<PhysicsMaterial::MixProperty::Multiply>("Multiply")
                  .withVariant<PhysicsMaterial::MixProperty::Minimum>("Minimum")
                  .withVariant<PhysicsMaterial::MixProperty::Average>("Average"));
}

CUBOS_REFLECT_IMPL(PhysicsMaterial)
{
    return cubos::core::ecs::TypeBuilder<PhysicsMaterial>("cubos::engine::PhysicsMaterial")
        .withField("friction", &PhysicsMaterial::friction)
        .withField("bounciness", &PhysicsMaterial::bounciness)
        .withField("frictionMix", &PhysicsMaterial::frictionMix)
        .withField("bouncinessMix", &PhysicsMaterial::bouncinessMix)
        .build();
}

CUBOS_REFLECT_IMPL(PhysicsBundle)
{
    return cubos::core::ecs::TypeBuilder<PhysicsBundle>("cubos::engine::PhysicsBundle")
        .withField("mass", &PhysicsBundle::mass)
        .withField("velocity", &PhysicsBundle::velocity)
        .withField("force", &PhysicsBundle::force)
        .withField("impulse", &PhysicsBundle::impulse)
        .withField("material", &PhysicsBundle::material)
        .build();
}

CUBOS_REFLECT_IMPL(Damping)
{
    return core::ecs::TypeBuilder<Damping>("cubos::engine::Damping").build();
}

void cubos::engine::physicsPlugin(Cubos& cubos)
{
    cubos.plugin(physicsFixedSubstepPlugin);

    cubos.resource<Damping>();

    cubos.component<Velocity>();
    cubos.component<Force>();
    cubos.component<Impulse>();
    cubos.component<Mass>();
    cubos.component<AccumulatedCorrection>();
    cubos.component<PhysicsMaterial>();
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

                cmds.add(ent, Mass{.mass = bundle.mass, .inverseMass = 1.0F / bundle.mass});
                cmds.add(ent, Velocity{.vec = bundle.velocity});
                cmds.add(ent, force);
                cmds.add(ent, impulse);
                cmds.add(ent, AccumulatedCorrection{});
                cmds.add(ent, bundle.material);
            }
        });

    cubos.tag(physicsApplyForcesTag);
}
