/// @file
/// @brief Contains the class for the Capsule shape.

#pragma once

namespace cubos::core::geom
{
    /// @brief Capsule shape.
    /// Can be a capsule or a sphere.
    struct Capsule
    {
        const float radius; ///< The radius of the capsule.
        const float lenght; ///< The lenght of the capsule.

        /// Constructs a sphere.
        /// @param radius The radius of the sphere.
        /// @returns The sphere.
        static Capsule sphere(float radius)
        {
            return {radius, 0.0f};
        }

        /// @return The height of the capsule.
        float height() const
        {
            return lenght + 2.0f * radius;
        }
    };
} // namespace cubos::core::geom
