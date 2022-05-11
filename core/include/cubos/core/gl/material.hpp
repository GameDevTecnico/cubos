#ifndef CUBOS_CORE_GL_MATERIAL_HPP
#define CUBOS_CORE_GL_MATERIAL_HPP

#include <cubos/core/memory/serializer.hpp>
#include <cubos/core/memory/deserializer.hpp>

#include <glm/glm.hpp>

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

        /// Serializes the material.
        /// @param serializer The serializer to use.
        void serialize(memory::Serializer& serializer) const;

        /// Deserializes the material.
        /// @param deserializer The deserializer to use.
        void deserialize(memory::Deserializer& deserializer);
    };
} // namespace cubos::core::gl

#endif // CUBOS_CORE_GL_MATERIAL_HPP
