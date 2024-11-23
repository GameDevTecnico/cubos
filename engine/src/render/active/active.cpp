#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/active/active.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Active)
{
    return core::ecs::TypeBuilder<Active>("cubos::engine::Active").withField("active", &Active::active).build();
}