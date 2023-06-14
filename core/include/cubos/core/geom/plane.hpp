/// @file
/// @brief Contains the class for the Plane shape.

#pragma once

#include <glm/glm.hpp>

namespace cubos::core::geom
{
    /// @brief Plane shape.
    struct Plane
    {
        const glm::vec3 normal; ///< The normal of the plane.
    };
} // namespace cubos::core::geom
