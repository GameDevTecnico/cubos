#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/ui/canvas/keep_pixel_size.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::UIKeepPixelSize)
{
    return cubos::core::ecs::TypeBuilder<UIKeepPixelSize>("cubos::engine::UIKeepPixelSize").build();
}
