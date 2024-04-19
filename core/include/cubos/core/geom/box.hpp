/// @file
/// @brief Class @ref cubos::core::geom::Box.
/// @ingroup core-geom

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::geom
{
    /// @brief Represents a box shape.
    /// @ingroup core-geom
    struct Box
    {
        CUBOS_REFLECT;

        glm::vec3 halfSize{0.5F}; ///< Half size of the box.

        /// @brief Computes two opposite corners of the box on the major diagonal.
        /// @param corners Array to store the two corners in.
        void diag(glm::vec3 corners[2]) const
        {
            corners[0] = {-halfSize.x, -halfSize.y, -halfSize.z};
            corners[1] = {halfSize.x, halfSize.y, halfSize.z};
        }

        /// @brief Computes four corners of the box, one for each diagonal.
        /// @param corners Array to store the three corners in.
        void corners4(glm::vec3 corners[4]) const
        {
            corners[0] = {halfSize.x, -halfSize.y, -halfSize.z};
            corners[1] = {-halfSize.x, halfSize.y, -halfSize.z};
            corners[2] = {-halfSize.x, -halfSize.y, halfSize.z};
            corners[3] = {halfSize.x, halfSize.y, halfSize.z};
        }

        /// @brief Computes the eight corners of the box, opposite corners are adjacent in the
        /// array.
        /// @param corners Array to store the eight corners in.
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

        static void normals(glm::vec3 normals[3])
        {
            normals[0] = {0.0F, 1.0F, 0.0F};
            normals[1] = {0.0F, 0.0F, 1.0F};
            normals[2] = {1.0F, 0.0F, 0.0F};
        }
    };
} // namespace cubos::core::geom
