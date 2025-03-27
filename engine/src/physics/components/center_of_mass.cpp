#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/physics/components/center_of_mass.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::CenterOfMass)
{
    return cubos::core::ecs::TypeBuilder<CenterOfMass>("cubos::engine::CenterOfMass")
        .withField("vec", &CenterOfMass::vec)
        .build();
}
