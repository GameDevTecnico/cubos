/// [Logging and tracing include]
#include <cubos/core/tel/logging.hpp>
#include <cubos/core/tel/tracing.hpp>

using cubos::core::tel::SpanManager;
/// [Logging and tracing include]

int main()
{
    cubos::core::tel::Logger::level(cubos::core::tel::Logger::Level::Trace);

    /// [Using macros]
    CUBOS_SPAN_INFO("main_span");
    // With this macro, a new RAII guard is created. When dropped, exits the span.
    // This indicates that we are in the span for the current lexical scope.
    // Logs and metrics from here will be associated with 'main' span.
    CUBOS_INFO("hello!");

    CUBOS_SPAN_TRACE("other_scope");
    CUBOS_INFO("hello again!");
    // ...
    /// [Using macros]

    /// [Manually]
    SpanManager::Span span("manual_span", CUBOS_SPAN_LEVEL_DEBUG, __FILE__, __LINE__);
    SpanManager::enter(&span);
    CUBOS_INFO("entered a manual span");

    SpanManager::exit();
    CUBOS_INFO("after exit manual span");
    /// [Manually]
}
