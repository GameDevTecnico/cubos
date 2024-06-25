/// @file
/// @brief Tracing macros.
/// @ingroup core-tel

#pragma once

#include <chrono>
#include <string>
#include <thread>

namespace cubos::core::tel
{
    class SpanManager
    {
    public:
        class Span
        {
        public:
            Span(const std::string& name, std::size_t level, const std::string& file, std::size_t line);
            ~Span();
            std::string name();

        private:
            std::thread::id id;                                                 ///< Thread id where span was invoked.
            std::string mName;                                                  ///< The name of the scope.
            std::string mFile;                                                  ///< The file from where it was invoked.
            std::size_t mLine;                                                  ///< The line from where it was invoked.
            std::chrono::time_point<std::chrono::high_resolution_clock> mStart; ///< Start time when constructed.
            int mLevel;
        };

        /// @brief Gets the current active span.
        /// @return Span* Pointer to the current active span.
        static Span* current();

        /// @brief Decides next span name.
        /// @example If current span is `foo` and a `bar` was invoked, this returns "foo:bar`.
        /// @return Returns span name.
        static std::string nextName(std::string name);

        /// @brief Gets the count of active spans.
        /// @return Span count.
        static std::size_t count();

        /// @brief Enters a new span.
        /// @param s Pointer to the span to be entered.
        static void enter(Span* s);

        /// @brief Exits the current active span.
        static void exit();

    private:
        SpanManager() = default;
    };
} // namespace cubos::core::tel

#define SPAN(a, b) SPAN_INNER(a, b)
#define SPAN_INNER(a, b) a##b

/// @brief Constructs a new info span.
/// @param name Span name.
#define CUBOS_SPAN_INFO(name)                                                                                          \
    SpanManager::Span SPAN(info_span_, __COUNTER__)                                                                    \
    {                                                                                                                  \
        name, 1, __FILE__, __LINE__                                                                                    \
    }

/// @brief Constructs a new trace span.
/// @param name Span name.
#define CUBOS_SPAN_TRACE(name)                                                                                         \
    SpanManager::Span SPAN(trace_span_, __COUNTER__)                                                                   \
    {                                                                                                                  \
        name, 2, __FILE__, __LINE__                                                                                    \
    }

/// @brief Constructs a new debug span.
/// @param name Span name.
#define CUBOS_SPAN_DEBUG(name)                                                                                         \
    SpanManager::Span SPAN(trace_span_, __COUNTER__)                                                                   \
    {                                                                                                                  \
        name, 3, __FILE__, __LINE__                                                                                    \
    }