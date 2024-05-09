
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/ui/canvas/horizontal_stretch.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::UIHorizontalStretch)
{
    return cubos::core::ecs::TypeBuilder<UIHorizontalStretch>("cubos::engine::UIHorizontalStretch")
        .withField("leftOffset", &UIHorizontalStretch::leftOffset)
        .withField("rightOffset", &UIHorizontalStretch::rightOffset)
        .build();
}
