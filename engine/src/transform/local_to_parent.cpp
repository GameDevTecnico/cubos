#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/transform/local_to_parent.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::LocalToParent)
{
    return core::ecs::TypeBuilder<LocalToParent>("cubos::engine::LocalToParent").ephemeral().wrap(&LocalToParent::mat);
}
