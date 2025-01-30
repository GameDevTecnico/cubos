#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/physics/components/velocity.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Velocity)
{
    return cubos::core::ecs::TypeBuilder<Velocity>("cubos::engine::Velocity").wrap(&Velocity::vec);
}
