#pragma once

#include <glm/glm.hpp>

namespace cubos::engine
{
    /// Describes a collision between two objects.
    struct Manifold
    {
        /// Normal vector of the collision.
        glm::dvec3 normal;

        /// How much the first object should move along the collision normal axis to resolve the collision.
        double depth;
    };
} // namespace cubos::engine
