#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/ui/text/text_stretch.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::UITextStretch)
{
    return cubos::core::ecs::TypeBuilder<UITextStretch>("cubos::engine::UITextStretch").build();
}
