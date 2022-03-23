#ifndef CUBOS_GL_VERTEX_HPP
#define CUBOS_GL_VERTEX_HPP

#include <cubos/memory/serializer.hpp>
#include <cubos/memory/deserializer.hpp>

#include <glm/glm.hpp>
#include <vector>

namespace cubos::gl
{
    class Grid;

    /// Represents a voxel vertex.
    struct Vertex
    {
        glm::ivec3 position; ///< The position of the vertex.
        glm::vec3 normal; ///< The normal of the vertex.
        uint16_t material; ///< The material index on the palette.

        /// Serializes the grid.
        /// @param serializer The serializer to use.
        void serialize(memory::Serializer& serializer) const;

        /// Deserializes the grid.
        /// @param deserializer The deserializer to use.
        void deserialize(memory::Deserializer& deserializer);
    };

    /// Triangulates a grid of voxels into an indexed mesh.
    /// @param grid The grid to triangulate.
    /// @param vertices The vertices of the mesh.
    /// @param indices The indices of the mesh.
    void triangulate(const Grid& grid, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
} // namespace cubos::gl

#endif // CUBOS_GL_VERTEX_HPP
