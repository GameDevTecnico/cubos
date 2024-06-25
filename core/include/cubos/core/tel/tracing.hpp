/// @file
/// @brief Tracing macros.
/// @ingroup core-tel

#pragma once

#include <string>

#include <cubos/core/tel/level.hpp>

#define CUBOS_TEL_SPAN_CONCAT_(prefix, suffix) prefix##suffix
#define CUBOS_TEL_SPAN_CONCAT(prefix, suffix) CUBOS_TEL_SPAN_CONCAT_(prefix, suffix)
#define CUBOS_TEL_SPAN_UNIQUE(prefix) CUBOS_TEL_SPAN_CONCAT(prefix##_, __COUNTER__)

/// @addtogroup core-tel
/// @{

/// @brief Constructs a new span with a specified level.
/// @param name Span name.
/// @param level Span level.
#define CUBOS_SPAN(name, level)                                                                                        \
    cubos::core::tel::SpanGuard CUBOS_TEL_SPAN_UNIQUE(spanGuard){};                                                    \
    cubos::core::tel::SpanManager::begin(name, level)

/// @brief Constructs a new info span.
/// @param name Span name.
#define CUBOS_SPAN_INFO(name) CUBOS_SPAN(name, cubos::core::tel::Level::Info)

/// @brief Constructs a new debug span.
/// @param name Span name.
#define CUBOS_SPAN_DEBUG(name) CUBOS_SPAN(name, cubos::core::tel::Level::Debug)

/// @brief Constructs a new trace span.
/// @param name Span name.
#define CUBOS_SPAN_TRACE(name) CUBOS_SPAN(name, cubos::core::tel::Level::Trace)

/// @}

namespace cubos::core::tel
{
    /// @brief Uniquely identifies a span.
    struct SpanId
    {
        std::size_t id;   ///< Identifier which distinguishes between instances of spans with the same path.
        std::string path; ///< Span path.
    };

    /// @brief A guard object that automatically ends the current span when it goes out of scope.
    class SpanGuard
    {
    public:
        SpanGuard() = default;
        ~SpanGuard();
    };

    /// @brief Manages the creation and ending of spans.
    class SpanManager
    {
    public:
        /// @brief Begins a new span.
        /// @param name Span name.
        /// @param level Span level.
        static void begin(const std::string& name, Level level);

        /// @brief Ends the current active span.
        static void end();

        /// @brief Gets the current active span.
        /// @return Identifier of the current span.
        static SpanId current();
    };
} // namespace cubos::core::tel
