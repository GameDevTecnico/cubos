/// @file
/// @brief Class @ref cubos::core::geom::Plane.
/// @ingroup core-geom

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/data/old/deserializer.hpp>
#include <cubos/core/data/old/serializer.hpp>

namespace cubos::core::geom
{
    /// @brief Represents a plane shape.
    /// @ingroup core-geom
    struct Plane
    {
        glm::vec3 normal{0.0F, 1.0F, 0.0F}; ///< Normal of the plane.
    };
} // namespace cubos::core::geom

namespace cubos::core::data
{
    inline void serialize(Serializer& ser, const geom::Plane& plane, const char* name)
    {
        ser.beginObject(name);
        ser.write(plane.normal, "normal");
        ser.endObject();
    }

    inline void deserialize(Deserializer& des, geom::Plane& plane)
    {
        des.beginObject();
        des.read(plane.normal);
        des.endObject();
    }
} // namespace cubos::core::data
