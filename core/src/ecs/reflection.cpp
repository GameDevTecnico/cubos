#include <cubos/core/ecs/reflection.hpp>

CUBOS_REFLECT_IMPL(cubos::core::ecs::SymmetricTrait)
{
    return reflection::Type::create("cubos::core::ecs::SymmetricTrait");
}

CUBOS_REFLECT_IMPL(cubos::core::ecs::TreeTrait)
{
    return reflection::Type::create("cubos::core::ecs::TreeTrait");
}

CUBOS_REFLECT_IMPL(cubos::core::ecs::EphemeralTrait)
{
    return reflection::Type::create("cubos::core::ecs::EphemeralTrait");
}
