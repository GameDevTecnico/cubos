#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/physics/components/angular_velocity.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::AngularVelocity)
{
    return cubos::core::ecs::TypeBuilder<AngularVelocity>("cubos::engine::AngularVelocity")
        .withField("vec", &AngularVelocity::vec)
        .build();
}
