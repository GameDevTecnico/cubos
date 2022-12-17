#ifndef CUBOS_ENGINE_ECS_SCALE_HPP
#define CUBOS_ENGINE_ECS_SCALE_HPP

#include <components/base.hpp>

#include <glm/glm.hpp>

namespace cubos::engine::ecs
{
    /// Assigns a uniform scale to an entity.
    struct [[cubos::component("cubos/scale", VecStorage)]] Scale
    {
        float factor; ///< The scale factor of the entity.
    };
} // namespace cubos::engine::ecs

#endif // CUBOS_ENGINE_ECS_SCALE_HPP
