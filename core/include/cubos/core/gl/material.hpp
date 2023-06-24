#pragma once

#include <glm/glm.hpp>

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>

namespace cubos::core::gl
{
    /// Represents a voxel material.
    struct Material
    {
        static const Material Empty; ///< The empty material.

        glm::vec4 color; ///< The color of the material.

        /// Compares this material with another and returns a number which indicates how similar they are to one
        /// another. The number is in the range [0, 1], where 0 means they are completely different and 1 means they
        /// are exactly the same.
        /// @param other The other material to compare with.
        /// @return The similarity between the two materials.
        float similarity(const Material& other) const;
    };
} // namespace cubos::core::gl

namespace cubos::core::old::data
{
    /// Serializes a material.
    /// @param serializer The serializer to use.
    /// @param mat The material to serialize.
    /// @param name The name of the material.
    void serialize(Serializer& serializer, const gl::Material& mat, const char* name);

    /// Deserializes a material.
    /// @param deserializer The deserializer to use.
    /// @param mat The material to deserialize.
    void deserialize(Deserializer& deserializer, gl::Material& mat);
} // namespace cubos::core::old::data
