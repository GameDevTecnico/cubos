/// [Logging and tracing include]
#include <cubos/core/tel/level.hpp>
#include <cubos/core/tel/logging.hpp>
#include <cubos/core/tel/tracing.hpp>

using cubos::core::tel::Level;
using cubos::core::tel::Logger;
using cubos::core::tel::SpanManager;
/// [Logging and tracing include]

int main()
{
    /// [Set level]
    cubos::core::tel::level(Level::Debug);
    /// [Set level]

    /// [Using macros]
    CUBOS_SPAN_TRACE("this_wont_exist!");

    CUBOS_INFO("hello from root span!");

    CUBOS_SPAN_INFO("main_span");
    // With this macro, a new RAII guard is created. When dropped, exits the span.
    // This indicates that we are in the span for the current lexical scope.
    // Logs and metrics from here will be associated with 'main' span.
    CUBOS_INFO("hello!");

    CUBOS_SPAN_DEBUG("other_scope");
    CUBOS_INFO("hello again!");
    // ...

    /// [Manually]
    SpanManager::begin("manual_span", cubos::core::tel::Level::Debug);
    CUBOS_INFO("entered a manual span");

    SpanManager::end();
    CUBOS_INFO("after exit manual span");
    /// [Manually]
}