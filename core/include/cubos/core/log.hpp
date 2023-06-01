#pragma once

#include <cstdlib>

#ifndef CUBOS_LOG_LEVEL
#ifndef NDEBUG
#define CUBOS_LOG_LEVEL 0
#else
#define CUBOS_LOG_LEVEL 2
#endif
#endif

#undef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL CUBOS_LOG_LEVEL
#include <spdlog/spdlog.h>

// The possible log levels.

#define CUBOS_LOG_LEVEL_TRACE SPDLOG_LEVEL_TRACE
#define CUBOS_LOG_LEVEL_DEBUG SPDLOG_LEVEL_DEBUG
#define CUBOS_LOG_LEVEL_INFO SPDLOG_LEVEL_INFO
#define CUBOS_LOG_LEVEL_WARN SPDLOG_LEVEL_WARN
#define CUBOS_LOG_LEVEL_ERROR SPDLOG_LEVEL_ERROR
#define CUBOS_LOG_LEVEL_CRITICAL SPDLOG_LEVEL_CRITICAL
#define CUBOS_LOG_LEVEL_OFF SPDLOG_LEVEL_OFF

// Macros for logging.
// These macros are only evaluated if CUBOS_LOG_LEVEL is set to a level that is equal or lower than the level of the
// macro.

#define CUBOS_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define CUBOS_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define CUBOS_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define CUBOS_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define CUBOS_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define CUBOS_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

#define CUBOS_FAIL(...)                                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        if constexpr (sizeof(#__VA_ARGS__) > 1)                                                                        \
        {                                                                                                              \
            CUBOS_CRITICAL("" __VA_ARGS__);                                                                            \
        }                                                                                                              \
        std::abort();                                                                                                  \
    } while (false)

#define CUBOS_UNREACHABLE(...)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        CUBOS_CRITICAL("Reached unreachable code");                                                                    \
        CUBOS_FAIL(__VA_ARGS__);                                                                                       \
    } while (false)

#define CUBOS_ASSERT(cond, ...)                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(cond))                                                                                                   \
        {                                                                                                              \
            CUBOS_CRITICAL("Assertion {} failed", #cond);                                                              \
            CUBOS_FAIL(__VA_ARGS__);                                                                                   \
        }                                                                                                              \
    } while (false)

// FIXME: this should compiled out when some option is set to false
#define CUBOS_DEBUG_ASSERT(cond, ...) CUBOS_ASSERT(cond)

namespace cubos::core
{
    /// Initializes the logger, must be called before any logging is done.
    void initializeLogger();

    /// Disables all logging except for critical errors.
    void disableLogging();
} // namespace cubos::core
