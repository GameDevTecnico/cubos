/// @file
/// @brief Class @ref cubos::engine::VoxelVertex and function @ref cubos::engine::triangulate.
/// @ingroup renderer-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    class VoxelGrid;

    /// @brief Represents a voxel vertex.
    /// @ingroup renderer-plugin
    struct CUBOS_ENGINE_API VoxelVertex
    {
        glm::uvec3 position; ///< Position of the vertex.
        glm::vec3 normal;    ///< Normal of the vertex.
        uint16_t material;   ///< Index of the material on the palette.
    };

    /// @brief Triangulates a grid of voxels into an indexed mesh.
    /// @param grid Grid to triangulate.
    /// @param vertices Vertices of the mesh.
    /// @param indices Indices of the mesh.
    /// @ingroup renderer-plugin
    CUBOS_ENGINE_API void triangulate(const VoxelGrid& grid, std::vector<VoxelVertex>& vertices,
                                      std::vector<uint32_t>& indices);
} // namespace cubos::engine
