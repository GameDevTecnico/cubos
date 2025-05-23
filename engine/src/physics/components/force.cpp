#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/physics/components/force.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Force)
{
    return cubos::core::ecs::TypeBuilder<Force>("cubos::engine::Force").build();
}
