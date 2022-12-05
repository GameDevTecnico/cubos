#include <cubos/core/log.hpp>
#include <cubos/core/memory/buffer_stream.hpp>
#include <cubos/core/data/debug_serializer.hpp>

using namespace cubos::core;

int main(void)
{
    initializeLogger();

    CUBOS_TRACE("Trace message");
    CUBOS_DEBUG("Debug message");
    CUBOS_INFO("Info message");
    CUBOS_WARN("Warning message");
    CUBOS_ERROR("Error message");
    CUBOS_CRITICAL("Critical message");

    CUBOS_INFO("Trivially serializable type: {}", glm::vec3(0.0f, 1.0f, 2.0f));
    CUBOS_INFO("Again, but with type information: {:t}", glm::vec3(0.0f, 1.0f, 2.0f));
    CUBOS_INFO("Since unordered maps are serializable, we can do this: {}",
               std::unordered_map<int, const char*>{{1, "one"}, {2, "two"}});

    return 0;
}
