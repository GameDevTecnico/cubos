#pragma once

#include <cubos/engine/collisions/manifold.hpp>

namespace cubos::engine
{
    /// Describes a spherical shape used to detect collisions.
    struct SphereShape
    {
        glm::dvec3 position; ///< Center position of the sphere.
        double radius;       ///< Radius of the sphere.
    };

    /// Checks if there is a collision between two spheres.
    /// @param sphere1 The first sphere.
    /// @param sphere2 The second sphere.
    /// @param manifold Output manifold describing the collision.
    /// @returns Whether there was a collision or not.
    bool checkCollision(const SphereShape& sphere1, const SphereShape& sphere2, Manifold& manifold);
} // namespace cubos::engine
