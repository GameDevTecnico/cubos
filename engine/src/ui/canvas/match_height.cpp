#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/ui/canvas/match_height.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::UIMatchHeight)
{
    return cubos::core::ecs::TypeBuilder<UIMatchHeight>("cubos::engine::UIMatchHeight").build();
}
