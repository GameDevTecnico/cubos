#ifndef CUBOS_CORE_GL_MATERIAL_HPP
#define CUBOS_CORE_GL_MATERIAL_HPP

#include <cubos/memory/serializer.hpp>
#include <cubos/memory/deserializer.hpp>

#include <glm/glm.hpp>

namespace cubos::core::gl
{
    /// Represents a voxel material.
    struct Material
    {
        static const Material Empty; ///< The empty material.

        glm::vec4 color; ///< The color of the material.

        /// Serializes the material.
        /// @param serializer The serializer to use.
        void serialize(memory::Serializer& serializer) const;

        /// Deserializes the material.
        /// @param deserializer The deserializer to use.
        void deserialize(memory::Deserializer& deserializer);
    };
} // namespace cubos::core::gl

#endif // CUBOS_CORE_GL_MATERIAL_HPP
