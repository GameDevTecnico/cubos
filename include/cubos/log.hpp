#ifndef CUBOS_LOG_HPP
#define CUBOS_LOG_HPP

#include <spdlog/spdlog.h>

namespace cubos
{
    void initializeLogger();

    template <typename T> void logTrace(T &msg)
    {
        spdlog::trace(msg);
    }

    template <typename... Args> void logTrace(fmt::format_string<Args...> fmt, Args &&...args)
    {
        spdlog::trace(fmt, std::forward<Args>(args)...);
    }

    template <typename T> void logDebug(T &msg)
    {
        spdlog::debug(msg);
    }

    template <typename... Args> void logDebug(fmt::format_string<Args...> fmt, Args &&...args)
    {
        spdlog::debug(fmt, std::forward<Args>(args)...);
    }

    template <typename T> void logInfo(T &msg)
    {
        spdlog::info(msg);
    }

    template <typename... Args> void logInfo(fmt::format_string<Args...> fmt, Args &&...args)
    {
        spdlog::info(fmt, std::forward<Args>(args)...);
    }

    template <typename T> void logWarning(T &msg)
    {
        spdlog::warn(msg);
    }

    template <typename... Args> void logWarning(fmt::format_string<Args...> fmt, Args &&...args)
    {
        spdlog::warn(fmt, std::forward<Args>(args)...);
    }

    template <typename T> void logError(T &msg)
    {
        spdlog::error(msg);
    }

    template <typename... Args> void logError(fmt::format_string<Args...> fmt, Args &&...args)
    {
        spdlog::error(fmt, std::forward<Args>(args)...);
    }

    template <typename T> void logCritical(T &msg)
    {
        spdlog::critical(msg);
    }

    template <typename... Args> void logCritical(fmt::format_string<Args...> fmt, Args &&...args)
    {
        spdlog::critical(fmt, std::forward<Args>(args)...);
    }
} // namespace cubos

#endif // CUBOS_LOG_HPP
