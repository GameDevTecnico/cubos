#ifndef CUBOS_CORE_GL_VERTEX_HPP
#define CUBOS_CORE_GL_VERTEX_HPP

#include <cubos/core/memory/serializer.hpp>
#include <cubos/core/memory/deserializer.hpp>

#include <glm/glm.hpp>

namespace cubos::core::gl
{
    class Grid;

    // Represents a voxel vertex
    struct Vertex
    {
        glm::uvec3 position; ///< The position of the vertex.

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
} // namespace cubos::core::gl

#endif // CUBOS_CORE_GL_VERTEX_HPP
