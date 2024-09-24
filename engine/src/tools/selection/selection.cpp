#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/tools/selection/selection.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Selection)
{
    return cubos::core::ecs::TypeBuilder<Selection>("cubos::engine::::Selection")
        .withField("entity", &Selection::entity)
        .build();
}
