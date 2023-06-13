#include <sstream>

#include <glm/gtx/io.hpp>

#define CHECK_GLM_EQ(a, ...) CHECK_MESSAGE((a == __VA_ARGS__), to_string(a), " != ", to_string(__VA_ARGS__))

template <typename T>
std::string to_string(const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}