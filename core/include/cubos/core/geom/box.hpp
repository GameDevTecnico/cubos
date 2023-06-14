/// @file
/// @brief Contains the class for the Box shape.

#pragma once

#include <glm/glm.hpp>

namespace cubos::core::geom
{
    /// @brief Box shape.
    struct Box
    {
        const glm::vec3 halfSize; ///< The half size of the box.

        /// Computes two opposite corners of the box.
        /// @param corners The array to store the two corners in.
        void corners2(glm::vec3 corners[2]) const
        {
            corners[0] = {-halfSize.x, -halfSize.y, -halfSize.z};
            corners[1] = {halfSize.x, halfSize.y, halfSize.z};
        }

        /// Computes six opposite corners of the box. Opposite corners are adjacent in the array.
        /// @param corners The array to store the six corners in.
        void corners6(glm::vec3 corners[6]) const
        {
            corners[0] = {-halfSize.x, -halfSize.y, -halfSize.z};
            corners[1] = {halfSize.x, halfSize.y, halfSize.z};
            corners[2] = {-halfSize.x, -halfSize.y, halfSize.z};
            corners[3] = {halfSize.x, halfSize.y, -halfSize.z};
            corners[4] = {-halfSize.x, halfSize.y, -halfSize.z};
            corners[5] = {halfSize.x, -halfSize.y, halfSize.z};
        }

        /// Computes the eight corners of the box. Opposite corners are adjacent in the array.
        /// @param corners The array to store the eight corners in.
        void corners(glm::vec3 corners[8]) const
        {
            corners[0] = {-halfSize.x, -halfSize.y, -halfSize.z};
            corners[1] = {halfSize.x, halfSize.y, halfSize.z};
            corners[2] = {-halfSize.x, -halfSize.y, halfSize.z};
            corners[3] = {halfSize.x, halfSize.y, -halfSize.z};
            corners[4] = {-halfSize.x, halfSize.y, -halfSize.z};
            corners[5] = {halfSize.x, -halfSize.y, halfSize.z};
            corners[6] = {-halfSize.x, halfSize.y, halfSize.z};
            corners[7] = {halfSize.x, -halfSize.y, -halfSize.z};
        }
    };
} // namespace cubos::core::geom
