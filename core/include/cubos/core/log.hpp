#ifndef CUBOS_CORE_LOG_HPP
#define CUBOS_CORE_LOG_HPP

#include <cubos/core/memory/buffer_stream.hpp>
#include <cubos/core/data/debug_serializer.hpp>

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

namespace cubos::core
{
    /// Initializes the logger, must be called before any logging is done.
    void initializeLogger();
} // namespace cubos::core

// Implement custom formatting for all trivially serializable types.

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

#endif // CUBOS_CORE_LOG_HPP
