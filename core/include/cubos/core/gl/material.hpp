/// @file
/// @brief Class @ref cubos::core::gl::Material.
/// @ingroup core-gl

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>

namespace cubos::core::gl
{
    /// @brief Describes a voxel material.
    /// @ingroup core-gl
    struct Material
    {
        static const Material Empty; ///< Empty material, used for voxels with index 0.

        glm::vec4 color; ///< Color of the material.

        /// @brief Compares this material with another and returns a number which indicates how
        /// similar they are.
        ///
        /// The number is in the range [0, 1], where 0 means they are completely different and 1
        /// means they are exactly the same.
        ///
        /// @param other Other material to compare with.
        /// @return Similarity between the two materials.
        float similarity(const Material& other) const;
    };
} // namespace cubos::core::gl

namespace cubos::core::data
{
    void serialize(Serializer& serializer, const gl::Material& mat, const char* name);
    void deserialize(Deserializer& deserializer, gl::Material& mat);
} // namespace cubos::core::data
