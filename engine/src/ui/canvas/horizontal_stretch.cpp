
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/ui/canvas/horizontal_stretch.hpp>

using namespace cubos::core::gl;

CUBOS_REFLECT_IMPL(cubos::engine::HorizontalStretch)
{
    return cubos::core::ecs::TypeBuilder<HorizontalStretch>("cubos::engine::HorizontalStretch")
        .withField("leftOffset", &HorizontalStretch::leftOffset)
        .withField("rightOffset", &HorizontalStretch::rightOffset)
        .build();
}
