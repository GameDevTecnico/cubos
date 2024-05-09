
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/ui/canvas/vertical_stretch.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::VerticalStretch)
{
    return cubos::core::ecs::TypeBuilder<VerticalStretch>("cubos::engine::VerticalStretch")
        .withField("topOffset", &VerticalStretch::topOffset)
        .withField("bottomOffset", &VerticalStretch::bottomOffset)
        .build();
}
