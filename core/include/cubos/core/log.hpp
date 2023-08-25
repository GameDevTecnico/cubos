/// @file
/// @brief Logging and assertion macros.
/// @ingroup core

#pragma once

#include <cstdlib>

/// @addtogroup core
/// @{

/// @def CUBOS_LOG_LEVEL
/// @brief Log level to compile in.
///
/// This macro essentially controls the minimum log level that will be compiled into the binary.
///
/// @todo The log level should be modifiable at runtime.
///
/// Should be set to one of the following:
/// - @ref CUBOS_LOG_LEVEL_TRACE
/// - @ref CUBOS_LOG_LEVEL_DEBUG
/// - @ref CUBOS_LOG_LEVEL_INFO
/// - @ref CUBOS_LOG_LEVEL_WARN
/// - @ref CUBOS_LOG_LEVEL_ERROR
/// - @ref CUBOS_LOG_LEVEL_CRITICAL
/// - @ref CUBOS_LOG_LEVEL_OFF
///
/// By default, on debug builds, this is set to @ref CUBOS_LOG_LEVEL_TRACE.
/// On release builds, this is set to @ref CUBOS_LOG_LEVEL_INFO.

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

/// @brief Trace log level, lowest log level. Very verbose.
/// @sa CUBOS_LOG_LEVEL
#define CUBOS_LOG_LEVEL_TRACE SPDLOG_LEVEL_TRACE

/// @brief Debug log level. Contains messages useful for debugging, but which are not necessary in
/// release builds.
/// @sa CUBOS_LOG_LEVEL
#define CUBOS_LOG_LEVEL_DEBUG SPDLOG_LEVEL_DEBUG

/// @brief Information log level. Contains important events that are not errors.
/// @sa CUBOS_LOG_LEVEL
#define CUBOS_LOG_LEVEL_INFO SPDLOG_LEVEL_INFO

/// @brief Warn log level. Contains events that are not errors, but which are unexpected and may be
/// problematic.
/// @sa CUBOS_LOG_LEVEL
#define CUBOS_LOG_LEVEL_WARN SPDLOG_LEVEL_WARN

/// @brief Error log level. Contains errors which are recoverable from.
/// @sa CUBOS_LOG_LEVEL
#define CUBOS_LOG_LEVEL_ERROR SPDLOG_LEVEL_ERROR

/// @brief Critical log level, highest log level. Contains errors which are unrecoverable from.
/// @sa CUBOS_LOG_LEVEL
#define CUBOS_LOG_LEVEL_CRITICAL SPDLOG_LEVEL_CRITICAL

/// @brief Off log level, disables all logging.
/// @sa CUBOS_LOG_LEVEL
#define CUBOS_LOG_LEVEL_OFF SPDLOG_LEVEL_OFF

/// @brief Used for logging very verbose information.
/// @param ... Format string and arguments.
/// @see CUBOS_LOG_LEVEL_TRACE
#define CUBOS_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)

/// @brief Used for logging information which is useful for debugging but not necessary in release
/// builds.
/// @param ... Format string and arguments.
/// @see CUBOS_LOG_LEVEL_DEBUG
#define CUBOS_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)

/// @brief Used for logging information which is useful in release builds.
/// @param ... Format string and arguments.
/// @see CUBOS_LOG_LEVEL_INFO
#define CUBOS_INFO(...) SPDLOG_INFO(__VA_ARGS__)

/// @brief Used for logging unexpected events.
/// @param ... Format string and arguments.
/// @see CUBOS_LOG_LEVEL_WARN
#define CUBOS_WARN(...) SPDLOG_WARN(__VA_ARGS__)

/// @brief Used for logging recoverable errors.
/// @param ... Format string and arguments.
/// @see CUBOS_LOG_LEVEL_ERROR
#define CUBOS_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)

/// @brief Used for logging unrecoverable errors.
/// @param ... Format string and arguments.
/// @see CUBOS_LOG_LEVEL_CRITICAL
#define CUBOS_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

/// @brief Aborts a program, optionally printing a critical error message.
/// @param ... Optional format string and arguments.
#define CUBOS_FAIL(...)                                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        if constexpr (sizeof(#__VA_ARGS__) > 1)                                                                        \
        {                                                                                                              \
            CUBOS_CRITICAL("" __VA_ARGS__);                                                                            \
        }                                                                                                              \
        std::abort();                                                                                                  \
    } while (false)

/// @brief Marks a code path as supposedly unreachable. Aborts the program when reached.
/// @param ... Optional format string and arguments.
#define CUBOS_UNREACHABLE(...)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        CUBOS_CRITICAL("Reached unreachable code");                                                                    \
        CUBOS_FAIL(__VA_ARGS__);                                                                                       \
    } while (false)

/// @brief Asserts that a condition is true, aborting the program if it is not.
/// @param cond Condition to assert.
/// @param ... Optional format string and arguments.
#define CUBOS_ASSERT(cond, ...)                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(cond))                                                                                                   \
        {                                                                                                              \
            CUBOS_CRITICAL("Assertion {} failed", #cond);                                                              \
            CUBOS_FAIL(__VA_ARGS__);                                                                                   \
        }                                                                                                              \
    } while (false)

/// @brief In debug builds asserts that a condition is true, aborting the program if it is not.
/// @param cond Condition to assert.
/// @param ... Optional format string and arguments.
/// @todo This should be compiled out when some option is set to false.
#define CUBOS_DEBUG_ASSERT(cond, ...) CUBOS_ASSERT(cond)

/// @}

namespace cubos::core
{
    /// @brief Must be called before any logging is done.
    /// @ingroup core
    void initializeLogger();

    /// @brief Disables all logging except for critical errors.
    /// @ingroup core
    void disableLogging();
} // namespace cubos::core
