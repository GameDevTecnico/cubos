#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/ui/canvas/expand.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::UIExpand)
{
    return cubos::core::ecs::TypeBuilder<UIExpand>("cubos::engine::UIExpand").build();
}
