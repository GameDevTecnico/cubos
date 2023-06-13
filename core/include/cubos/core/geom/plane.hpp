#pragma once

#include <glm/glm.hpp>

namespace cubos::core::geom
{
    struct Plane
    {
        glm::vec3 normal; ///< The normal of the plane.
    };
} // namespace cubos::core::geom