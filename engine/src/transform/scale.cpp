#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/transform/scale.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Scale)
{
    return core::ecs::TypeBuilder<Scale>("cubos::engine::Scale").wrap(&Scale::factor);
}
