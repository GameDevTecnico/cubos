#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/transform/rotation.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Rotation)
{
    return core::ecs::TypeBuilder<Rotation>("cubos::engine::Rotation").withField("quat", &Rotation::quat).build();
}
