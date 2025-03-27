#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/physics/components/torque.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Torque)
{
    return cubos::core::ecs::TypeBuilder<Torque>("cubos::engine::Torque").build();
}
