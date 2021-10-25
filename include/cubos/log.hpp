#ifndef CUBOS_LOG_HPP
#define CUBOS_LOG_HPP

#include <spdlog/spdlog.h>

namespace cubos
{
    /// Initializes the logger, must be called before any logging is done
    void initializeLogger();

    /// Logs a trace message
    /// @tparam T Message type
    /// @param msg Message
    template <typename T> void logTrace(T& msg)
    {
        spdlog::trace(msg);
    }

    /// Logs a trace message
    /// @tparam Args Format arguments types
    /// @param fmt Format string
    /// @param args Format arguments
    template <typename... Args> void logTrace(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::trace(fmt, std::forward<Args>(args)...);
    }

    /// Logs a debug message
    /// @tparam T Message type
    /// @param msg Message
    template <typename T> void logDebug(T& msg)
    {
        spdlog::debug(msg);
    }

    /// Logs a debug message
    /// @tparam Args Format arguments types
    /// @param fmt Format string
    /// @param args Format arguments
    template <typename... Args> void logDebug(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::debug(fmt, std::forward<Args>(args)...);
    }

    /// Logs a info message
    /// @tparam T Message type
    /// @param msg Message
    template <typename T> void logInfo(T& msg)
    {
        spdlog::info(msg);
    }

    /// Logs a info message
    /// @tparam Args Format arguments types
    /// @param fmt Format string
    /// @param args Format arguments
    template <typename... Args> void logInfo(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::info(fmt, std::forward<Args>(args)...);
    }

    /// Logs a warning message
    /// @tparam T Message type
    /// @param msg Message
    template <typename T> void logWarning(T& msg)
    {
        spdlog::warn(msg);
    }

    /// Logs a warning message
    /// @tparam Args Format arguments types
    /// @param fmt Format string
    /// @param args Format arguments
    template <typename... Args> void logWarning(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::warn(fmt, std::forward<Args>(args)...);
    }

    /// Logs a error message
    /// @tparam T Message type
    /// @param msg Message
    template <typename T> void logError(T& msg)
    {
        spdlog::error(msg);
    }

    /// Logs a error message
    /// @tparam Args Format arguments types
    /// @param fmt Format string
    /// @param args Format arguments
    template <typename... Args> void logError(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::error(fmt, std::forward<Args>(args)...);
    }

    /// Logs a critical message
    /// @tparam T Message type
    /// @param msg Message
    template <typename T> void logCritical(T& msg)
    {
        spdlog::critical(msg);
    }

    /// Logs a critical message
    /// @tparam Args Format arguments types
    /// @param fmt Format string
    /// @param args Format arguments
    template <typename... Args> void logCritical(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::critical(fmt, std::forward<Args>(args)...);
    }
} // namespace cubos

#endif // CUBOS_LOG_HPP
