/// @file
/// @brief Contains the AABB resource.

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/geom/simplex.hpp>
#include <cubos/core/log.hpp>

namespace cubos::engine
{
    /// @brief The AABB of a collider.
    struct [[cubos::component("cubos/aabb", VecStorage)]] ColliderAABB
    {
        /// The diagonal of the AABB.
        core::geom::Simplex diag = core::geom::Simplex::line(glm::vec3{-INFINITY}, glm::vec3{INFINITY});

        /// Sets the minimum point of the AABB.
        /// @param min The new minimum point.
        void min(const glm::vec3& min)
        {
            CUBOS_ASSERT(min.x < diag.points[1].x && min.y < diag.points[1].y && min.z < diag.points[1].z,
                         "AABB minimum point must be less than maximum point");
            diag.points[0] = min;
        }

        /// @return The minimum point of the AABB.
        const glm::vec3& min() const
        {
            return diag.points[0];
        }

        /// Sets the maximum point of the AABB.
        /// @param max The new maximum point.
        void max(const glm::vec3& max)
        {
            CUBOS_ASSERT(max.x > diag.points[0].x && max.y > diag.points[0].y && max.z > diag.points[0].z,
                         "AABB maximum point must be greater than minimum point");
            diag.points[1] = max;
        }

        /// @return The maximum point of the AABB.
        const glm::vec3& max() const
        {
            return diag.points[1];
        }
    };
} // namespace cubos::engine