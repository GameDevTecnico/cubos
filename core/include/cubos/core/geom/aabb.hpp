/// @file
/// @brief Component @ref cubos::core::geom::AABB.
/// @ingroup core-geom

#pragma once

#include <glm/mat4x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <cubos/core/geom/box.hpp>

namespace cubos::core::geom
{
    /// @brief Represents an axis-aligned bounding box.
    /// @ingroup core-geom
    struct CUBOS_CORE_API AABB
    {
        /// @brief Diagonal of the AABB.
        glm::vec3 diag[2] = {glm::vec3{-std::numeric_limits<float>::infinity()},
                             glm::vec3{-std::numeric_limits<float>::infinity()}};

        /// @brief Minimum point of the AABB.
        /// @return Minimum point of the AABB.
        glm::vec3 min() const
        {
            return diag[0];
        }

        /// @brief Maximum point of the AABB.
        /// @return Maximum point of the AABB.
        glm::vec3 max() const
        {
            return diag[1];
        }

        /// @brief Sets the minimum point of the AABB.
        /// @param min Minimum point of the AABB.
        void min(const glm::vec3& min)
        {
            diag[0] = min;
        }

        /// @brief Sets the maximum point of the AABB.
        /// @param max Maximum point of the AABB.
        void max(const glm::vec3& max)
        {
            diag[1] = max;
        }

        /// @brief Gets a @ref Box representation of the AABB.
        /// @return @ref Box representation.
        Box box() const
        {
            auto size = max() - min();
            return Box{size / 2.0F};
        }

        /// @brief Gets the center of the AABB.
        /// @return Center of the AABB.
        glm::vec3 center() const
        {
            return (min() + max()) / 2.0F;
        }

        /// @brief Checks if the AABB overlaps with another AABB on the X axis.
        /// @param other Other AABB.
        /// @return Whether the AABBs overlap.
        bool overlapsX(const AABB& other) const
        {
            return min().x <= other.max().x && max().x >= other.min().x;
        }

        /// @brief Checks if the AABB overlaps with another AABB on the Y axis.
        /// @param other Other AABB.
        /// @return Whether the AABBs overlap.
        bool overlapsY(const AABB& other) const
        {
            return min().y <= other.max().y && max().y >= other.min().y;
        }

        /// @brief Checks if the AABB overlaps with another AABB on the Z axis.
        /// @param other Other AABB.
        /// @return Whether the AABBs overlap.
        bool overlapsZ(const AABB& other) const
        {
            return min().z <= other.max().z && max().z >= other.min().z;
        }

        /// @brief Checks if the AABB overlaps with another AABB.
        /// @param other Other AABB.
        /// @return Whether the AABBs overlap.
        bool overlaps(const AABB& other) const
        {
            return overlapsX(other) && overlapsY(other) && overlapsZ(other);
        }

        /// @brief Generates an AABB from a box and its transform.
        /// @param box Box.
        /// @param transform Transform to apply to the box.
        /// @return The AABB of the box.
        static AABB fromOBB(const Box& box, glm::mat4 transform)
        {
            // Get the 4 points of the box.
            glm::vec3 corners[4];
            box.corners4(corners);

            // Pack the 4 points of the box into a matrix.
            auto points = glm::mat4{glm::vec4{corners[0], 1.0F}, glm::vec4{corners[1], 1.0F},
                                    glm::vec4{corners[2], 1.0F}, glm::vec4{corners[3], 1.0F}};

            // We only want scale and rotation, extract translation and remove it.
            auto translation = glm::vec3{transform[3]};
            transform[3] = glm::vec4{0.0F, 0.0F, 0.0F, 1.0F};

            // Rotate and scale corners.
            auto rotatedCorners = glm::mat4x3{transform * points};

            // Get the max of the rotated corners.
            auto max = glm::max(glm::abs(rotatedCorners[0]), glm::abs(rotatedCorners[1]));
            max = glm::max(max, glm::abs(rotatedCorners[2]));
            max = glm::max(max, glm::abs(rotatedCorners[3]));

            // Set the AABB.
            AABB aabb{};
            aabb.min(translation - max);
            aabb.max(translation + max);
            return aabb;
        }
    };
} // namespace cubos::core::geom
