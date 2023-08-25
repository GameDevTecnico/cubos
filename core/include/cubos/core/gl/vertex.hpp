/// @file
/// @brief Class @ref cubos::core::gl::Vertex and function @ref cubos::core::gl::triangulate.
/// @ingroup core-gl

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>

namespace cubos::core::gl
{
    class Grid;

    /// @brief Represents a voxel vertex.
    /// @ingroup core-gl
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
    /// @ingroup core-gl
    void triangulate(const Grid& grid, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
} // namespace cubos::core::gl

namespace cubos::core::data
{
    void serialize(Serializer& serializer, const gl::Vertex& vertex, const char* name);
    void deserialize(Deserializer& deserializer, gl::Vertex& vertex);
} // namespace cubos::core::data
