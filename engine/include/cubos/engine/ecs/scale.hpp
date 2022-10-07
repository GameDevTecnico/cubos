#ifndef CUBOS_ENGINE_ECS_SCALE_HPP
#define CUBOS_ENGINE_ECS_SCALE_HPP

#include <cubos/core/ecs/vec_storage.hpp>

#include <glm/glm.hpp>

namespace cubos::engine::ecs
{
    /// Assigns a uniform scale to an entity.
    struct Scale
    {
        using Storage = core::ecs::VecStorage<Scale>;

        float factor; ///< The scale factor of the entity.
    };

} // namespace cubos::engine::ecs

#endif // CUBOS_ENGINE_ECS_SCALE_HPP
