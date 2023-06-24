/// @file
/// @brief Contains the class for the Plane shape.

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>

namespace cubos::core::geom
{
    /// @brief Plane shape.
    struct Plane
    {
        glm::vec3 normal{0.0f, 1.0f, 0.0f}; ///< The normal of the plane.
    };
} // namespace cubos::core::geom

namespace cubos::core::old::data
{
    /// Serializes a plane.
    /// @param ser The serializer to use.
    /// @param plane The plane to serialize.
    /// @param name The name of the plane.
    inline void serialize(Serializer& ser, const geom::Plane& plane, const char* name)
    {
        ser.beginObject(name);
        ser.write(plane.normal, "normal");
        ser.endObject();
    }

    /// Deserializes a plane.
    /// @param des The deserializer to use.
    /// @param plane The plane to deserialize.
    inline void deserialize(Deserializer& des, geom::Plane& plane)
    {
        des.beginObject();
        des.read(plane.normal);
        des.endObject();
    }
} // namespace cubos::core::old::data
