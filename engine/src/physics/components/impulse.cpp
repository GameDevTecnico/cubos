#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/physics/components/impulse.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Impulse)
{
    return cubos::core::ecs::TypeBuilder<Impulse>("cubos::engine::Impulse").build();
}
