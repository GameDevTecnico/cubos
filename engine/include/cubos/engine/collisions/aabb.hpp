/// @file
/// @brief Component @ref cubos::engine::ColliderAABB.
/// @ingroup collisions-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/data/old/deserializer.hpp>
#include <cubos/core/data/old/serializer.hpp>
#include <cubos/core/geom/box.hpp>
#include <cubos/core/log.hpp>

namespace cubos::engine
{
    /// @brief Component which stores the AABB of an entity with a collider component.
    /// @ingroup collisions-plugin
    struct [[cubos::component("cubos/aabb", VecStorage)]] ColliderAABB
    {
        /// @brief Minimum point of the diagonal of the AABB.
        glm::vec3 min = glm::vec3{-INFINITY};

        /// @brief Maximum point of the diagonal of the AABB.
        glm::vec3 max = glm::vec3{INFINITY};

        /// @brief Gets a @ref core::geom::Box representation of the AABB.
        /// @return @ref core::geom::Box representation.
        core::geom::Box box() const
        {
            auto size = max - min;
            return core::geom::Box{size / 2.0F};
        }

        /// @brief Gets the center of the AABB.
        /// @return Center of the AABB.
        glm::vec3 center() const
        {
            return (min + max) / 2.0F;
        }

        /// @brief Checks if the AABB overlaps with another AABB on the X axis.
        /// @param other Other AABB.
        /// @return Whether the AABBs overlap.
        bool overlapsX(const ColliderAABB& other) const
        {
            return min.x <= other.max.x && max.x >= other.min.x;
        }

        /// @brief Checks if the AABB overlaps with another AABB on the Y axis.
        /// @param other Other AABB.
        /// @return Whether the AABBs overlap.
        bool overlapsY(const ColliderAABB& other) const
        {
            return min.y <= other.max.y && max.y >= other.min.y;
        }

        /// @brief Checks if the AABB overlaps with another AABB on the Z axis.
        /// @param other Other AABB.
        /// @return Whether the AABBs overlap.
        bool overlapsZ(const ColliderAABB& other) const
        {
            return min.z <= other.max.z && max.z >= other.min.z;
        }

        /// @brief Checks if the AABB overlaps with another AABB.
        /// @param other Other AABB.
        /// @return Whether the AABBs overlap.
        bool overlaps(const ColliderAABB& other) const
        {
            return overlapsX(other) && overlapsY(other) && overlapsZ(other);
        }
    };
} // namespace cubos::engine

namespace cubos::core::data
{
    inline void serialize(Serializer& ser, const engine::ColliderAABB& aabb, const char* name)
    {
        ser.beginObject(name);
        ser.write(aabb.min, "min");
        ser.write(aabb.max, "max");
        ser.endObject();
    }

    inline void deserialize(Deserializer& des, engine::ColliderAABB& aabb)
    {
        des.beginObject();
        des.read(aabb.min);
        des.read(aabb.max);
        des.endObject();
    }
} // namespace cubos::core::data
