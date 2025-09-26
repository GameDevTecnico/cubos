#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/physics/rigid_body_bundle.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::RigidBodyBundle)
{
    return cubos::core::ecs::TypeBuilder<RigidBodyBundle>("cubos::engine::RigidBodyBundle")
        .withField("isActive", &RigidBodyBundle::isActive)
        .withField("layers", &RigidBodyBundle::layers)
        .withField("mask", &RigidBodyBundle::mask)
        .withField("mass", &RigidBodyBundle::mass)
        .withField("velocity", &RigidBodyBundle::velocity)
        .withField("angularVelocity", &RigidBodyBundle::angularVelocity)
        .withField("force", &RigidBodyBundle::force)
        .withField("torque", &RigidBodyBundle::torque)
        .withField("impulse", &RigidBodyBundle::impulse)
        .withField("angularImpulse", &RigidBodyBundle::angularImpulse)
        .withField("material", &RigidBodyBundle::material)
        .withField("centerOfMass", &RigidBodyBundle::centerOfMass)
        .withField("autoInertiaTensor", &RigidBodyBundle::autoInertiaTensor)
        .withField("inertiaTensor", &RigidBodyBundle::inertiaTensor)
        .build();
}