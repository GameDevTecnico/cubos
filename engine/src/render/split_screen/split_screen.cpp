#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/render/split_screen/split_screen.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::SplitScreen)
{
    return core::ecs::TypeBuilder<SplitScreen>("cubos::engine::SplitScreen").build();
}
