#ifndef CUBOS_CORE_GL_VERTEX_HPP
#define CUBOS_CORE_GL_VERTEX_HPP

#include <glm/glm.hpp>

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>

namespace cubos::core::gl
{
    class Grid;

    /// Represents a voxel vertex.
    struct Vertex
    {
        glm::uvec3 position; ///< The position of the vertex.
        glm::vec3 normal;    ///< The normal of the vertex.
        uint16_t material;   ///< The index of the material on the palette.
    };

    /// Triangulates a grid of voxels into an indexed mesh.
    /// @param grid The grid to triangulate.
    /// @param vertices The vertices of the mesh.
    /// @param indices The indices of the mesh.
    void triangulate(const Grid& grid, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
} // namespace cubos::core::gl

namespace cubos::core::data
{
    /// Serializes a voxel vertex.
    /// @param serializer The serializer to use.
    /// @param vertex The vertex to serialize.
    /// @param name The name of the vertex.
    void serialize(Serializer& serializer, const gl::Vertex& vertex, const char* name);

    /// Deserializes a voxel vertex.
    /// @param deserializer The deserializer to use.
    /// @param vertex The vertex to deserialize.
    void deserialize(Deserializer& deserializer, gl::Vertex& vertex);
} // namespace cubos::core::data

#endif // CUBOS_CORE_GL_VERTEX_HPP
