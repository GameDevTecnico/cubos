#include <glm/glm.hpp>

#include <cubos/core/data/debug_serializer.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/memory/buffer_stream.hpp>

using namespace cubos::core;
using old::data::Debug;

int main()
{
    initializeLogger();

    CUBOS_TRACE("Trace message");
    CUBOS_DEBUG("Debug message");
    CUBOS_INFO("Info message");
    CUBOS_WARN("Warning message");
    CUBOS_ERROR("Error message");
    CUBOS_CRITICAL("Critical message");

    CUBOS_INFO("Serializable type: {}", Debug(glm::vec3(0.0F, 1.0F, 2.0F)));
    CUBOS_INFO("Again, but with type information: {:t}", Debug(glm::vec3(0.0F, 1.0F, 2.0F)));
    CUBOS_INFO("Since unordered maps are serializable, we can do this: {}",
               Debug((std::unordered_map<int, const char*>{{1, "one"}, {2, "two"}})));

    return 0;
}
