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

struct TypeWithoutReflection
{
};

int main()
{
    /// [Set logging level]
    cubos::core::Logger::level(cubos::core::Logger::Level::Trace);
    /// [Set logging level]

    /// [Logging macros]
    CUBOS_TRACE("Trace message");
    CUBOS_DEBUG("Debug message");
    CUBOS_INFO("Info message");
    CUBOS_WARN("Warning message");
    CUBOS_ERROR("Error message");
    CUBOS_CRITICAL("Critical message");
    /// [Logging macros]

    /// [Logging macros with arguments]
    CUBOS_INFO("An integer: {}", 1);
    CUBOS_INFO("A glm::vec3: {}", glm::vec3(0.0F, 1.0F, 2.0F));
    CUBOS_INFO("An std::unordered_map: {}", std::unordered_map<int, const char*>{{1, "one"}, {2, "two"}});
    CUBOS_INFO("A type without reflection: {}", TypeWithoutReflection{});
}
/// [Logging macros with arguments]
