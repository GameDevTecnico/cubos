
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/ui/canvas/vertical_stretch.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::UIVerticalStretch)
{
    return cubos::core::ecs::TypeBuilder<UIVerticalStretch>("cubos::engine::UIVerticalStretch")
        .withField("topOffset", &UIVerticalStretch::topOffset)
        .withField("bottomOffset", &UIVerticalStretch::bottomOffset)
        .build();
}
