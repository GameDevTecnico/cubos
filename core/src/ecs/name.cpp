#include <cubos/core/ecs/name.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/string.hpp>

CUBOS_REFLECT_IMPL(cubos::core::ecs::Name)
{
    return core::ecs::TypeBuilder<Name>("cubos::core::ecs::Name").ephemeral().withField("value", &Name::value).build();
}
