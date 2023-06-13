#include <glm/gtx/string_cast.hpp>

#define CHECK_GLM_EQ(a, ...) CHECK_MESSAGE((a == __VA_ARGS__), glm::to_string(a), " != ", glm::to_string(__VA_ARGS__))