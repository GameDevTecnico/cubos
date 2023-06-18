/// @file
/// @brief Contains the AABB resource.

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>
#include <cubos/core/log.hpp>

namespace cubos::engine
{
    /// @brief The AABB of a collider.
    struct [[cubos::component("cubos/aabb", VecStorage)]] ColliderAABB
    {
        /// The diagonal of the AABB.
        glm::vec3 min = glm::vec3{-INFINITY};
        glm::vec3 max = glm::vec3{INFINITY};

        /// @return Whether the AABB overlaps with another AABB on the X axis.
        /// @param other The other AABB.
        bool overlapsX(const ColliderAABB& other) const
        {
            return min.x <= other.max.x && max.x >= other.min.x;
        }

        /// @return Whether the AABB overlaps with another AABB on the Y axis.
        /// @param other The other AABB.
        bool overlapsY(const ColliderAABB& other) const
        {
            return min.y <= other.max.y && max.y >= other.min.y;
        }

        /// @return Whether the AABB overlaps with another AABB on the Z axis.
        /// @param other The other AABB.
        bool overlapsZ(const ColliderAABB& other) const
        {
            return min.z <= other.max.z && max.z >= other.min.z;
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
        ser.beginObject(name);
        ser.write(aabb.min, "min");
        ser.write(aabb.max, "max");
        ser.endObject();
    }

    /// Deserializes a AABB.
    /// @param des The deserializer to use.
    /// @param aabb The AABB to deserialize.
    inline void deserialize(Deserializer& des, engine::ColliderAABB& aabb)
    {
        des.beginObject();
        des.read(aabb.min);
        des.read(aabb.max);
        des.endObject();
    }
} // namespace cubos::core::data