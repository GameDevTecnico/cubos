#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/renderer/viewport.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Viewport)
{
    return core::ecs::TypeBuilder<Viewport>("cubos::engine::Viewport")
        .withField("position", &Viewport::position)
        .withField("size", &Viewport::size)
        .build();
}
