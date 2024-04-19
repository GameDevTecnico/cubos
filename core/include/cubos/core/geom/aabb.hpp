/// @file
/// @brief Component @ref cubos::core::geom::AABB.
/// @ingroup core-geom

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/geom/box.hpp>

namespace cubos::core::geom
{
    /// @brief Represents an axis-aligned bounding box.
    /// @ingroup core-geom
    struct AABB
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
    };
} // namespace cubos::core::geom
