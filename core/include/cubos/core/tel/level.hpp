/// @file
/// @brief Telemetry levels.
/// @ingroup core-tel

#pragma once

#include <cubos/core/api.hpp>
#include <cubos/core/reflection/reflect.hpp>

/// @addtogroup core-tel
/// @{

/// @brief Trace telemetry level, lowest telemetry level. Very verbose.
/// @sa CUBOS_TEL_LEVEL
#define CUBOS_TEL_LEVEL_TRACE 0

/// @brief Debug telemetry level. Contains logs and tracing useful for debugging, but which are not necessary in
/// release builds.
/// @sa CUBOS_TEL_LEVEL
#define CUBOS_TEL_LEVEL_DEBUG 1

/// @brief Information telemetry level. Contains important events that are not errors.
/// @sa CUBOS_TEL_LEVEL
#define CUBOS_TEL_LEVEL_INFO 2

/// @brief Warn telemetry level. Contains events that are not errors, but which are unexpected and may be
/// problematic.
/// @sa CUBOS_TEL_LEVEL
#define CUBOS_TEL_LEVEL_WARN 3

/// @brief Error telemetry level. Contains errors which are recoverable from.
/// @sa CUBOS_TEL_LEVEL
#define CUBOS_TEL_LEVEL_ERROR 4

/// @brief Critical telemetry level, highest log level. Contains errors which are unrecoverable from.
/// @sa CUBOS_TEL_LEVEL
#define CUBOS_TEL_LEVEL_CRITICAL 5

/// @brief Off telemetry level, disables all logging/tracing.
/// @sa CUBOS_TEL_LEVEL
#define CUBOS_TEL_LEVEL_OFF 6

/// @def CUBOS_TEL_LEVEL
/// @brief Telemetry level to compile in.
///
/// This macro essentially controls the minimum Telemetry level that will be compiled into the binary.
///
/// @todo The Telemetry level should be modifiable at runtime.
///
/// Should be set to one of the following:
/// - @ref CUBOS_TEL_LEVEL_TRACE
/// - @ref CUBOS_TEL_LEVEL_DEBUG
/// - @ref CUBOS_TEL_LEVEL_INFO
/// - @ref CUBOS_TEL_LEVEL_WARN
/// - @ref CUBOS_TEL_LEVEL_ERROR
/// - @ref CUBOS_TEL_LEVEL_CRITICAL
/// - @ref CUBOS_TEL_LEVEL_OFF
///
/// By default, on debug builds, this is set to @ref CUBOS_TEL_LEVEL_TRACE.
/// On release builds, this is set to @ref CUBOS_TEL_LEVEL_INFO.

#ifndef CUBOS_TEL_LEVEL
#ifndef NDEBUG
#define CUBOS_TEL_LEVEL CUBOS_TEL_LEVEL_TRACE
#else
#define CUBOS_TEL_LEVEL CUBOS_TEL_LEVEL_INFO
#endif
#endif

namespace cubos::core::tel
{
    /// @brief Represents a telemetry level.
    enum class Level
    {
        /// @copybrief CUBOS_TEL_LEVEL_TRACE
        Trace = CUBOS_TEL_LEVEL_TRACE,

        /// @copybrief CUBOS_TEL_LEVEL_DEBUG
        Debug = CUBOS_TEL_LEVEL_DEBUG,

        /// @copybrief CUBOS_TEL_LEVEL_INFO
        Info = CUBOS_TEL_LEVEL_INFO,

        /// @copybrief CUBOS_TEL_LEVEL_WARN
        Warn = CUBOS_TEL_LEVEL_WARN,

        /// @copybrief CUBOS_TEL_LEVEL_ERROR
        Error = CUBOS_TEL_LEVEL_ERROR,

        /// @copybrief CUBOS_TEL_LEVEL_CRITICAL
        Critical = CUBOS_TEL_LEVEL_CRITICAL,

        /// @copybrief CUBOS_TEL_LEVEL_OFF
        Off = CUBOS_TEL_LEVEL_OFF,
    };

    /// @brief Set telemetry level.
    /// @param level level
    void level(Level level);

    /// @brief Get telemetry level.
    /// @return level
    auto level() -> Level;
} // namespace cubos::core::tel

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, cubos::core::tel::Level);
