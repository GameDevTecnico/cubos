/// @file
/// @brief Class @ref cubos::core::geom::Capsule.
/// @ingroup core-geom

#pragma once

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>

namespace cubos::core::geom
{
    /// @brief Represents a capsule or sphere shape.
    /// @ingroup core-geom
    struct Capsule
    {
        float radius = 1.0F; ///< Radius of the capsule.
        float length = 0.0F; ///< Length of the capsule.

        /// @brief Constructs a sphere.
        /// @param radius Sphere radius.
        /// @return Sphere shape.
        static Capsule sphere(float radius)
        {
            return {radius, 0.0F};
        }

        /// @brief Gets the height of the capsule.
        /// @return Height of the capsule.
        float height() const
        {
            return length + 2.0F * radius;
        }
    };
} // namespace cubos::core::geom

namespace cubos::core::data
{
    inline void serialize(Serializer& ser, const geom::Capsule& capsule, const char* name)
    {
        ser.beginObject(name);
        ser.write(capsule.radius, "radius");
        ser.write(capsule.length, "length");
        ser.endObject();
    }

    inline void deserialize(Deserializer& des, geom::Capsule& capsule)
    {
        des.beginObject();
        des.read(capsule.radius);
        des.read(capsule.length);
        des.endObject();
    }
} // namespace cubos::core::data
