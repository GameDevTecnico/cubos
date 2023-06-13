#pragma once

#include <vector>

namespace cubos::core::geom
{
    struct Capsule
    {
        const float radius;        ///< The radius of the capsule.
        const float lenght = 0.0f; ///< The lenght of the capsule.

        /// @return The height of the capsule.
        float height() const
        {
            return lenght + 2.0f * radius;
        }
    };
} // namespace cubos::core::geom