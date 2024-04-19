/// @file
/// @brief Class @ref cubos::core::geom::Capsule.
/// @ingroup core-geom

#pragma once

#include <cubos/core/geom/aabb.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::geom
{
    /// @brief Represents a capsule or sphere shape.
    /// @ingroup core-geom
    struct Capsule
    {
        CUBOS_REFLECT;

        float radius = 1.0F; ///< Radius of the capsule.
        float length = 0.0F; ///< Length of the capsule.

        /// @brief Constructs a sphere.
        /// @param radius Sphere radius.
        /// @return Sphere shape.
        static Capsule sphere(float radius)
        {
            return {radius, 0.0F};
        }

        /// @brief Gets the height of the capsule.
        /// @return Height of the capsule.
        float height() const
        {
            return length + 2.0F * radius;
        }

        /// @brief Computes the local AABB of the capsule.
        /// @return Local AABB of the capsule.
        AABB aabb() const
        {
            AABB aabb;
            aabb.min({-radius, -radius - (length / 2.0F), -radius});
            aabb.max({radius, radius + (length / 2.0F), radius});
            return aabb;
        }
    };
} // namespace cubos::core::geom
