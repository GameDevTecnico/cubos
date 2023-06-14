/// @file
/// @brief Contains the class for the Capsule shape.

#pragma once

namespace cubos::core::geom
{
    /// @brief Capsule shape, which can also represent a sphere.
    struct Capsule
    {
        const float radius; ///< The radius of the capsule.
        const float length; ///< The length of the capsule.

        /// @brief Constructs a sphere.
        /// @param radius The radius of the sphere.
        /// @returns The sphere.
        static Capsule sphere(float radius)
        {
            return {radius, 0.0f};
        }

        /// @return The height of the capsule.
        float height() const
        {
            return length + 2.0f * radius;
        }
    };
} // namespace cubos::core::geom
