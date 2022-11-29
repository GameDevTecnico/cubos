#ifndef CUBOS_ENGINE_ECS_LOCAL_TO_WORLD_HPP
#define CUBOS_ENGINE_ECS_LOCAL_TO_WORLD_HPP

#include <components/base.hpp>

#include <glm/glm.hpp>

namespace cubos::engine::ecs
{
    /// A matrix calculated from the position, rotation and scale of an entity.
    /// This matrix should
    struct [[cubos::component("cubos/local_to_world", VecStorage)]] LocalToWorld
    {
        glm::mat4 mat = glm::mat4(1.0f); ///< The local to world matrix.
    };

} // namespace cubos::engine::ecs

#endif // CUBOS_ENGINE_ECS_LOCAL_TO_WORLD_HPP
