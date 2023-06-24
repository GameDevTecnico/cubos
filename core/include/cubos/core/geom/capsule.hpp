/// @file
/// @brief Contains the class for the Capsule shape.

#pragma once

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>

namespace cubos::core::geom
{
    /// @brief Capsule shape, which can also represent a sphere.
    struct Capsule
    {
        float radius = 1.0f; ///< The radius of the capsule.
        float length = 0.0f; ///< The length of the capsule.

        /// @brief Constructs a sphere.
        /// @param radius The radius of the sphere.
        /// @returns The sphere.
        static Capsule sphere(float radius)
        {
            return {radius, 0.0f};
        }

        /// @return The height of the capsule.
        float height() const
        {
            return length + 2.0f * radius;
        }
    };
} // namespace cubos::core::geom

namespace cubos::core::old::data
{
    /// Serializes a capsule.
    /// @param ser The serializer to use.
    /// @param capsule The capsule to serialize.
    /// @param name The name of the capsule.
    inline void serialize(Serializer& ser, const geom::Capsule& capsule, const char* name)
    {
        ser.beginObject(name);
        ser.write(capsule.radius, "radius");
        ser.write(capsule.length, "length");
        ser.endObject();
    }

    /// Deserializes a capsule.
    /// @param des The deserializer to use.
    /// @param capsule The capsule to deserialize.
    inline void deserialize(Deserializer& des, geom::Capsule& capsule)
    {
        des.beginObject();
        des.read(capsule.radius);
        des.read(capsule.length);
        des.endObject();
    }
} // namespace cubos::core::old::data
