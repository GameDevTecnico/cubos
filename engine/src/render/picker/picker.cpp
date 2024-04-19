#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/picker/picker.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::RenderPicker)
{
    return core::ecs::TypeBuilder<RenderPicker>("cubos::engine::RenderPicker")
        .withField("size", &RenderPicker::size)
        .withField("cleared", &RenderPicker::cleared)
        .build();
}
