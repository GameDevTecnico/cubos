/// @file
/// @brief Contains the AABB resource.

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>

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

        /// @return Whether the AABB overlaps with another AABB on the X axis.
        /// @param other The other AABB.
        bool overlapsX(const ColliderAABB& other) const
        {
            return diag.points[0].x <= other.diag.points[1].x && diag.points[1].x >= other.diag.points[0].x;
        }

        /// @return Whether the AABB overlaps with another AABB on the Y axis.
        /// @param other The other AABB.
        bool overlapsY(const ColliderAABB& other) const
        {
            return diag.points[0].y <= other.diag.points[1].y && diag.points[1].y >= other.diag.points[0].y;
        }

        /// @return Whether the AABB overlaps with another AABB on the Z axis.
        /// @param other The other AABB.
        bool overlapsZ(const ColliderAABB& other) const
        {
            return diag.points[0].z <= other.diag.points[1].z && diag.points[1].z >= other.diag.points[0].z;
        }

        /// @return Whether the AABB overlaps with another AABB.
        /// @param other The other AABB.
        bool overlaps(const ColliderAABB& other) const
        {
            return overlapsX(other) && overlapsY(other) && overlapsZ(other);
        }
    };
} // namespace cubos::engine


namespace cubos::core::data
{
    /// Serializes an AABB.
    /// @param ser The serializer to use.
    /// @param aabb The AABB to serialize.
    /// @param name The name of the AABB.
    inline void serialize(Serializer& ser, const engine::ColliderAABB& aabb, const char* name)
    {
        ser.write(aabb.diag, name);
    }

    /// Deserializes a AABB.
    /// @param des The deserializer to use.
    /// @param aabb The AABB to deserialize.
    inline void deserialize(Deserializer& des, engine::ColliderAABB& aabb)
    {
        des.read(aabb.diag);
    }
} // namespace cubos::core::data