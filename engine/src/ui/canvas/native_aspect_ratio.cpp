
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/ui/canvas/native_aspect_ratio.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::UINativeAspectRatio)
{
    return cubos::core::ecs::TypeBuilder<UINativeAspectRatio>("cubos::engine::UINativeAspectRatio")
        .withField("ratio", &UINativeAspectRatio::ratio)
        .build();
}
