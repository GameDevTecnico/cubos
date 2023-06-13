#pragma once

#include <glm/glm.hpp>

namespace cubos::core::geom
{
    struct Plane
    {
        const glm::vec3 normal; ///< The normal of the plane.
    };
} // namespace cubos::core::geom