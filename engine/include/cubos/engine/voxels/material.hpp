/// @file
/// @brief Class @ref cubos::engine::VoxelMaterial.
/// @ingroup voxels-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Describes a voxel material.
    /// @ingroup voxels-plugin
    struct CUBOS_ENGINE_API VoxelMaterial
    {
        static const VoxelMaterial Empty; ///< Empty material, used for voxels with index 0.

        glm::vec4 color; ///< Color of the material.

        /// @brief Compares this material with another and returns a number which indicates how
        /// similar they are.
        ///
        /// The number is in the range [0, 1], where 0 means they are completely different and 1
        /// means they are exactly the same.
        ///
        /// @param other Other material to compare with.
        /// @return Similarity between the two materials.
        float similarity(const VoxelMaterial& other) const;
    };
} // namespace cubos::engine
