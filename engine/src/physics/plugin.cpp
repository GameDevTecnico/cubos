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

CUBOS_REFLECT_IMPL(CenterOfMass)
{
    return cubos::core::ecs::TypeBuilder<CenterOfMass>("cubos::engine::CenterOfMass")
        .withField("vec", &CenterOfMass::vec)
        .build();
}

CUBOS_REFLECT_IMPL(Inertia)
{
    return cubos::core::ecs::TypeBuilder<Inertia>("cubos::engine::Inertia")
        .withField("Inertia", &Inertia::inertia)
        .withField("inverseInertia", &Inertia::inverseInertia)
        .build();
}

CUBOS_REFLECT_IMPL(Velocity)
{
    return cubos::core::ecs::TypeBuilder<Velocity>("cubos::engine::Velocity")
        .withField("velocity", &Velocity::vec)
        .build();
}

CUBOS_REFLECT_IMPL(AngularVelocity)
{
    return cubos::core::ecs::TypeBuilder<AngularVelocity>("cubos::engine::AngularVelocity")
        .withField("vec", &AngularVelocity::vec)
        .build();
}

CUBOS_REFLECT_IMPL(Force)
{
    return cubos::core::ecs::TypeBuilder<Force>("cubos::engine::Force")
        .withField("mForce", &Force::mForce)
        .withField("mTorque", &Force::mTorque)
        .build();
}

CUBOS_REFLECT_IMPL(Torque)
{
    return cubos::core::ecs::TypeBuilder<Torque>("cubos::engine::Torque")
        .withField("mTorque", &Torque::mTorque)
        .build();
}

CUBOS_REFLECT_IMPL(Impulse)
{
    return cubos::core::ecs::TypeBuilder<Impulse>("cubos::engine::Impulse")
        .withField("mImpulse", &Impulse::mImpulse)
        .withField("mAngularImpulse", &Impulse::mAngularImpulse)
        .build();
}

CUBOS_REFLECT_IMPL(AngularImpulse)
{
    return cubos::core::ecs::TypeBuilder<AngularImpulse>("cubos::engine::AngularImpulse")
        .withField("mAngularImpulse", &AngularImpulse::mAngularImpulse)
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
        .withField("colliderDimensions", &PhysicsBundle::colliderDimensions)
        .withField("velocity", &PhysicsBundle::velocity)
        .withField("angularVelocity", &PhysicsBundle::angularVelocity)
        .withField("force", &PhysicsBundle::force)
        .withField("torque", &PhysicsBundle::torque)
        .withField("impulse", &PhysicsBundle::impulse)
        .withField("angularImpulse", &PhysicsBundle::angularImpulse)
        .withField("material", &PhysicsBundle::material)
        .build();
}

CUBOS_REFLECT_IMPL(Damping)
{
    return core::ecs::TypeBuilder<Damping>("cubos::engine::Damping").build();
}

// Compute Inertia Tensor for box shape
glm::mat3 boxInertiaTensor(float mass, glm::vec3 dimensions)
{
    float constant = mass / 12.0F;
    auto x2 = (float)glm::pow(dimensions.x, 2);
    auto y2 = (float)glm::pow(dimensions.y, 2);
    auto z2 = (float)glm::pow(dimensions.z, 2);

    return glm::mat3{constant * (z2 + y2), 0.0F, 0.0F, 0.0F, constant * (x2 + z2), 0.0F, 0.0F, 0.0F,
                     constant * (x2 + y2)};
}

void cubos::engine::physicsPlugin(Cubos& cubos)
{
    cubos.plugin(physicsFixedSubstepPlugin);

    cubos.resource<Damping>();

    cubos.component<Velocity>();
    cubos.component<AngularVelocity>();
    cubos.component<Force>();
    cubos.component<Torque>();
    cubos.component<Impulse>();
    cubos.component<AngularImpulse>();
    cubos.component<Mass>();
    cubos.component<Inertia>();
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

                auto torque = Torque{};
                torque.add(bundle.torque);

                auto impulse = Impulse{};
                impulse.add(bundle.impulse);

                auto angularImpulse = AngularImpulse{};
                angularImpulse.add(bundle.angularImpulse);

                cmds.add(ent, Mass{.mass = bundle.mass, .inverseMass = 1.0F / bundle.mass});

                glm::mat3 inertiaTensor = boxInertiaTensor(bundle.mass, bundle.colliderDimensions);
                cmds.add(ent, Inertia{.inertia = inertiaTensor, .inverseInertia = glm::inverse(inertiaTensor)});
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

    cubos.tag(physicsApplyForcesTag);
}
