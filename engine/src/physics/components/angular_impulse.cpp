#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/physics/components/angular_impulse.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::AngularImpulse)
{
    return cubos::core::ecs::TypeBuilder<AngularImpulse>("cubos::engine::AngularImpulse").build();
}
