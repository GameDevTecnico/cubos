#pragma once

#include <glm/glm.hpp>

namespace cubos::core::geom
{
    struct Ray
    {
        const glm::vec3 direction;        ///< The direction of the ray.
        const bool bidirectional = false; ///< Whether the ray is bidirectional.
    };
} // namespace cubos::core::geom