#include <glm/glm.hpp>

/// [Logging include]
#include <cubos/core/log.hpp>
/// [Logging include]

/// [External reflection includes]
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
/// [External reflection includes]

int main()
{
    /// [Set logging level]
    cubos::core::Logger::level(cubos::core::Logger::Level::Trace);
    /// [Set logging level]

    /// [Log to file]
    cubos::core::Logger::logToFile();
    /// [Log to file]

    /// [Logging macros]
    CUBOS_TRACE("Trace message");
    CUBOS_DEBUG("Debug message");
    CUBOS_INFO("Info message");
    CUBOS_WARN("Warning message");
    CUBOS_ERROR("Error message");
    CUBOS_CRITICAL("Critical message");
    /// [Logging macros]

    /// [Set logging file]
    cubos::core::Logger::logToFile("/logs/sample_logs.txt");
    /// [Set logging file]

    /// [Logging macros with arguments]
    CUBOS_INFO("An integer: {}", 1);
    CUBOS_INFO("A glm::vec3: {}", glm::vec3(0.0F, 1.0F, 2.0F));
    CUBOS_INFO("An std::unordered_map: {}", std::unordered_map<int, const char*>{{1, "one"}, {2, "two"}});
}
/// [Logging macros with arguments]
