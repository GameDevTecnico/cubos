#include <cubos/core/log.hpp>
#include <cubos/core/memory/buffer_stream.hpp>
#include <cubos/core/data/debug_serializer.hpp>

using namespace cubos::core;

int main(void)
{
    initializeLogger();

    logTrace("Trace message");
    logDebug("Debug message");
    logInfo("Info message");
    logWarning("Warning message");
    logError("Error message");
    logCritical("Critical message");

    logInfo("Trivially serializable type: {}", glm::vec3(0.0f, 1.0f, 2.0f));
    logInfo("Again, but with type information: {:t}", glm::vec3(0.0f, 1.0f, 2.0f));
    logInfo("Since unordered maps are serializable, we can do this: {}",
            std::unordered_map<int, const char*>{{1, "one"}, {2, "two"}});

    return 0;
}
