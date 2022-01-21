#ifndef CUBOS_GL_VERTEX_HPP
#define CUBOS_GL_VERTEX_HPP

#include <glm/glm.hpp>

namespace cubos::gl
{
    // Represents a voxel vertex
    struct Vertex
    {
        glm::uvec3 position; ///< The position of the vertex.

        glm::vec3 normal; ///< The normal of the vertex.

        uint16_t material; ///< The material index on the palette.
    };
} // namespace cubos::gl

#endif // CUBOS_GL_VERTEX_HPP
