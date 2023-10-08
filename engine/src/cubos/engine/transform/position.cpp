#include <cubos/core/ecs/component/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/transform/position.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Position)
{
    return core::ecs::ComponentTypeBuilder<Position>("cubos::engine::Position")
        .withField("vec", &Position::vec)
        .build();
}
