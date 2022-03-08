#include <cubos/gl/vertex.hpp>

using namespace cubos;
using namespace cubos::gl;

bool Vertex::operator==(const Vertex& v) const
{
    return position == v.position && normal == v.normal && material == v.material;
}

size_t Vertex::hash::operator()(const Vertex& vertex) const
{
    size_t hash_res = std::hash<glm::uint>()(vertex.position.x);
    hash_res = hash_res ^ std::hash<glm::uint>()(vertex.position.y) << 1;
    hash_res = hash_res ^ std::hash<glm::uint>()(vertex.position.z) << 1;
    hash_res = hash_res ^ std::hash<glm::uint>()(vertex.normal.x) << 1;
    hash_res = hash_res ^ std::hash<glm::uint>()(vertex.normal.y) << 1;
    hash_res = hash_res ^ std::hash<glm::uint>()(vertex.normal.z) << 1;
    hash_res = hash_res ^ std::hash<uint16_t>()(vertex.material) << 1;
    return hash_res;
}
