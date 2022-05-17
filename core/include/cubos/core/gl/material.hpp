#ifndef CUBOS_CORE_GL_MATERIAL_HPP
#define CUBOS_CORE_GL_MATERIAL_HPP

#include <cubos/core/data/serializer.hpp>
#include <cubos/core/data/deserializer.hpp>

#include <glm/glm.hpp>

namespace cubos::core::gl
{
    /// Represents a voxel material.
    struct Material
    {
        static const Material Empty; ///< The empty material.

        glm::vec4 color; ///< The color of the material.
    };
} // namespace cubos::core::gl

namespace cubos::core::data
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
} // namespace cubos::core::data

#endif // CUBOS_CORE_GL_MATERIAL_HPP
