#include <cubos/core/ecs/component/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/renderer/viewport.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Viewport)
{
    return core::ecs::ComponentTypeBuilder<Viewport>("cubos::engine::Viewport")
        .withField("position", &Viewport::position)
        .withField("size", &Viewport::size)
        .build();
}
