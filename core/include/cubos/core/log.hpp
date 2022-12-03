#ifndef CUBOS_CORE_LOG_HPP
#define CUBOS_CORE_LOG_HPP

#include <cubos/core/memory/buffer_stream.hpp>
#include <cubos/core/data/debug_serializer.hpp>

#include <spdlog/spdlog.h>

namespace cubos::core
{
    /// Initializes the logger, must be called before any logging is done.
    void initializeLogger();

    /// Logs a trace message.
    /// @tparam T Message type.
    /// @param msg Message.
    template <typename T> void logTrace(T& msg)
    {
        spdlog::trace(msg);
    }

    /// Logs a trace message.
    /// @tparam Args Format arguments types.
    /// @param fmt Format string.
    /// @param args Format arguments.
    template <typename... Args> void logTrace(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::trace(fmt, std::forward<Args>(args)...);
    }

    /// Logs a debug message.
    /// @tparam T Message type.
    /// @param msg Message.
    template <typename T> void logDebug(T& msg)
    {
        spdlog::debug(msg);
    }

    /// Logs a debug message.
    /// @tparam Args Format arguments types.
    /// @param fmt Format string.
    /// @param args Format arguments.
    template <typename... Args> void logDebug(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::debug(fmt, std::forward<Args>(args)...);
    }

    /// Logs a info message.
    /// @tparam T Message type.
    /// @param msg Message.
    template <typename T> void logInfo(T& msg)
    {
        spdlog::info(msg);
    }

    /// Logs a info message.
    /// @tparam Args Format arguments types.
    /// @param fmt Format string.
    /// @param args Format arguments.
    template <typename... Args> void logInfo(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::info(fmt, std::forward<Args>(args)...);
    }

    /// Logs a warning message.
    /// @tparam T Message type.
    /// @param msg Message.
    template <typename T> void logWarning(T& msg)
    {
        spdlog::warn(msg);
    }

    /// Logs a warning message.
    /// @tparam Args Format arguments types.
    /// @param fmt Format string.
    /// @param args Format arguments.
    template <typename... Args> void logWarning(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::warn(fmt, std::forward<Args>(args)...);
    }

    /// Logs a error message.
    /// @tparam T Message type.
    /// @param msg Message.
    template <typename T> void logError(T& msg)
    {
        spdlog::error(msg);
    }

    /// Logs a error message.
    /// @tparam Args Format arguments types.
    /// @param fmt Format string.
    /// @param args Format arguments.
    template <typename... Args> void logError(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::error(fmt, std::forward<Args>(args)...);
    }

    /// Logs a critical message.
    /// @tparam T Message type.
    /// @param msg Message.
    template <typename T> void logCritical(T& msg)
    {
        spdlog::critical(msg);
    }

    /// Logs a critical message.
    /// @tparam Args Format arguments types.
    /// @param fmt Format string.
    /// @param args Format arguments.
    template <typename... Args> void logCritical(fmt::format_string<Args...> fmt, Args&&... args)
    {
        spdlog::critical(fmt, std::forward<Args>(args)...);
    }
} // namespace cubos::core

// Implement custom formatting for all trivially serializable types.

/// @cond
template <typename T>
requires(cubos::core::data::TriviallySerializable<T> && !std::is_pointer_v<T> && !std::is_array_v<T> &&
         !std::integral<T> && !std::floating_point<T> && !std::is_same_v<T, std::string>) struct fmt::formatter<T>
    : formatter<string_view>
{
    bool pretty = false; ///< Whether to pretty print the data.
    bool types = false;  ///< Whether to print the type name.

    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();
        while (it != end)
        {
            if (*it == 'p')
                this->pretty = true;
            else if (*it == 't')
                this->types = true;
            else if (*it != '}')
                throw format_error("invalid format");
            else
                break;
            ++it;
        }
        return it;
    }

    template <typename FormatContext> auto format(const T& val, FormatContext& ctx) -> decltype(ctx.out())
    {
        auto stream = cubos::core::memory::BufferStream(32);
        cubos::core::data::DebugSerializer serializer(stream, this->pretty, this->types);
        serializer.write(val, nullptr);
        stream.put('\0');
        // Skip the '?: ' prefix.
        auto result = std::string(static_cast<const char*>(stream.getBuffer()) + 3);
        return formatter<string_view>::format(string_view(result), ctx);
    }
};
/// @endcond

#endif // CUBOS_CORE_LOG_HPP
