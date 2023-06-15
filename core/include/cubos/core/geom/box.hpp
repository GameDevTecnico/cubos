/// @file
/// @brief Contains the class for the Box shape.

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>

namespace cubos::core::geom
{
    /// @brief Box shape.
    struct Box
    {
        glm::vec3 halfSize{0.5f}; ///< The half size of the box.

        /// @brief Computes two opposite corners of the box on the major diagonal.
        /// @param corners The array to store the two corners in.
        void diag(glm::vec3 corners[2]) const
        {
            corners[0] = {-halfSize.x, -halfSize.y, -halfSize.z};
            corners[1] = {halfSize.x, halfSize.y, halfSize.z};
        }

        /// @brief Computes three corners of the box, one for each non-major diagonal.
        /// @param corners The array to store the three corners in.
        void corners3(glm::vec3 corners[6]) const
        {
            corners[0] = {halfSize.x, -halfSize.y, -halfSize.z};
            corners[1] = {-halfSize.x, halfSize.y, -halfSize.z};
            corners[2] = {-halfSize.x, -halfSize.y, halfSize.z};
        }

        /// @brief Computes the eight corners of the box, opposite corners are adjacent in the array.
        /// @param corners The array to store the eight corners in.
        void corners(glm::vec3 corners[8]) const
        {
            corners[0] = {-halfSize.x, -halfSize.y, -halfSize.z};
            corners[1] = {halfSize.x, halfSize.y, halfSize.z};
            corners[2] = {-halfSize.x, -halfSize.y, halfSize.z};
            corners[3] = {halfSize.x, halfSize.y, -halfSize.z};
            corners[4] = {-halfSize.x, halfSize.y, -halfSize.z};
            corners[5] = {halfSize.x, -halfSize.y, halfSize.z};
            corners[6] = {-halfSize.x, halfSize.y, halfSize.z};
            corners[7] = {halfSize.x, -halfSize.y, -halfSize.z};
        }
    };
} // namespace cubos::core::geom

namespace cubos::core::data
{
    /// Serializes a box.
    /// @param ser The serializer to use.
    /// @param box The box to serialize.
    /// @param name The name of the plane.
    void serialize(Serializer& ser, const geom::Box& box, const char* name)
    {
        ser.beginObject(name);
        ser.write(box.halfSize, "halfSize");
        ser.endObject();
    }

    /// Deserializes a box.
    /// @param des The deserializer to use.
    /// @param box The box to deserialize.
    void deserialize(Deserializer& des, geom::Box& box)
    {
        des.beginObject();
        des.read(box.halfSize);
        des.endObject();
    }
} // namespace cubos::core::data
