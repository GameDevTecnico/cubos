
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/ui/color_rect/color_rect.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::UIColorRect)
{
    return cubos::core::ecs::TypeBuilder<UIColorRect>("cubos::engine::UIColorRect")
        .withField("color", &UIColorRect::color)
        .build();
}
