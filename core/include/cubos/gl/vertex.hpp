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

        bool operator==(const Vertex& lhs, const Vertex& rhs)
        {
            return lhs.position == rhs.position && lhs.normal == rhs.normal && lhs.material == rhs.normal;
        }

        struct hash
        {
            std::size_t operator()(const cubos::gl::Vertex& vertex) const
            {
                std::size_t hash_res = std::hash<glm::uint>()(vertex.position.x);
                hash_res = hash_res ^ std::hash<glm::uint>()(vertex.position.y) << 1;
                hash_res = hash_res ^ std::hash<glm::uint>()(vertex.position.z) << 1;
                hash_res = hash_res ^ std::hash<glm::uint>()(vertex.normal.x) << 1;
                hash_res = hash_res ^ std::hash<glm::uint>()(vertex.normal.y) << 1;
                hash_res = hash_res ^ std::hash<glm::uint>()(vertex.normal.z) << 1;
                hash_res = hash_res ^ std::hash<uint16_t>()(vertex.material) << 1;
                return hash_res;
            }
        };
    };
} // namespace cubos::gl

#endif // CUBOS_GL_VERTEX_HPP
