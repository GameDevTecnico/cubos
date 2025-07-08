#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/physics/components/interpolated.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(cubos::engine::Interpolated)
{
    return cubos::core::ecs::TypeBuilder<Interpolated>("cubos::engine::Interpolated").build();
}