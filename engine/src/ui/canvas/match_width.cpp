#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/ui/canvas/match_width.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::UIMatchWidth)
{
    return cubos::core::ecs::TypeBuilder<UIMatchWidth>("cubos::engine::UIMatchWidth").build();
}
