#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/physics/components/mass.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Mass)
{
    return cubos::core::ecs::TypeBuilder<Mass>("cubos::engine::Mass")
        .withField("mass", &Mass::mass)
        .withField("inverseMass", &Mass::inverseMass)
        .withField("changed", &Mass::changed)
        .build();
}
