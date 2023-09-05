#include <glm/glm.hpp>

/// [Logging include]
#include <cubos/core/log.hpp>
/// [Logging include]

/// [Debug wrapper include]
#include <cubos/core/data/debug_serializer.hpp>

using namespace cubos::core::data;
/// [Debug wrapper include]

/// [Logger initialization]
int main()
{
    cubos::core::initializeLogger();
    /// [Logger initialization]

    /// [Logging macros]
    CUBOS_TRACE("Trace message");
    CUBOS_DEBUG("Debug message");
    CUBOS_INFO("Info message");
    CUBOS_WARN("Warning message");
    CUBOS_ERROR("Error message");
    CUBOS_CRITICAL("Critical message");
    /// [Logging macros]

    /// [Logging macros with arguments]
    CUBOS_ERROR("Error message with {} argument", 1);
    /// [Logging macros with arguments]

    /// [Debug wrapper usage]
    CUBOS_INFO("Serializable type: {}", Debug(glm::vec3(0.0F, 1.0F, 2.0F)));
    CUBOS_INFO("Again, but with type information: {:t}", Debug(glm::vec3(0.0F, 1.0F, 2.0F)));
    CUBOS_INFO("Since unordered maps are serializable, we can do this: {}",
               Debug((std::unordered_map<int, const char*>{{1, "one"}, {2, "two"}})));
}
/// [Debug wrapper usage]
