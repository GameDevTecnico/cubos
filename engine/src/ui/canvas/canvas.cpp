#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/ui/canvas/canvas.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::UICanvas)
{
    return cubos::core::ecs::TypeBuilder<UICanvas>("cubos::engine::UICanvas")
        .withField("referenceSize", &UICanvas::referenceSize)
        .build();
}
