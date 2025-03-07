#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/physics/components/physics_material.hpp>

CUBOS_REFLECT_EXTERNAL_IMPL(cubos::engine::PhysicsMaterial::MixProperty)
{
    return cubos::core::reflection::Type::create("cubos::engine::PhysicsMaterial::MixProperty")
        .with(cubos::core::reflection::EnumTrait{}
                  .withVariant<cubos::engine::PhysicsMaterial::MixProperty::Maximum>("Maximum")
                  .withVariant<cubos::engine::PhysicsMaterial::MixProperty::Multiply>("Multiply")
                  .withVariant<cubos::engine::PhysicsMaterial::MixProperty::Minimum>("Minimum")
                  .withVariant<cubos::engine::PhysicsMaterial::MixProperty::Average>("Average"));
}

CUBOS_REFLECT_IMPL(cubos::engine::PhysicsMaterial)
{
    return cubos::core::ecs::TypeBuilder<PhysicsMaterial>("cubos::engine::PhysicsMaterial")
        .withField("friction", &PhysicsMaterial::friction)
        .withField("bounciness", &PhysicsMaterial::bounciness)
        .withField("frictionMix", &PhysicsMaterial::frictionMix)
        .withField("bouncinessMix", &PhysicsMaterial::bouncinessMix)
        .build();
}
