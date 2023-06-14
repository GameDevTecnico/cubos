#include <sstream>

#include <glm/glm.hpp>

#define CHECK_VEC3_EQ(a, ...) CHECK_MESSAGE((a == __VA_ARGS__), vec_to_string(a), " != ", vec_to_string(__VA_ARGS__))

inline std::string vec_to_string(const glm::vec3& v)
{
    std::stringstream ss;
    ss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return ss.str();
}
