#pragma once

#include <vector>

namespace cubos::core::geom
{
    struct Capsule
    {
        const float radius; ///< The radius of the capsule.
        const float lenght; ///< The lenght of the capsule.

        static const Capsule sphere(float radius)
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
