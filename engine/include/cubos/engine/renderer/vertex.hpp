/// @file
/// @brief Class @ref cubos::engine::Vertex and function @ref cubos::engine::triangulate.
/// @ingroup renderer-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>

namespace cubos::engine
{
    class Grid;

    /// @brief Represents a voxel vertex.
    /// @ingroup renderer-plugin
    struct Vertex
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
    void triangulate(const Grid& grid, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
} // namespace cubos::engine

namespace cubos::core::data
{
    void serialize(Serializer& serializer, const engine::Vertex& vertex, const char* name);
    void deserialize(Deserializer& deserializer, engine::Vertex& vertex);
} // namespace cubos::core::data
