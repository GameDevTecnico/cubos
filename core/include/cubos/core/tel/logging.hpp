/// @file
/// @brief Logging and assertion macros.
/// @ingroup core-tel

#pragma once

#include <cubos/core/memory/buffer_stream.hpp>
#include <cubos/core/reflection/reflect.hpp>

/// @addtogroup core-tel
/// @{

/// @brief Trace log level, lowest log level. Very verbose.
/// @sa CUBOS_LOG_LEVEL
#define CUBOS_LOG_LEVEL_TRACE 0

/// @brief Debug log level. Contains messages useful for debugging, but which are not necessary in
/// release builds.
/// @sa CUBOS_LOG_LEVEL
#define CUBOS_LOG_LEVEL_DEBUG 1

/// @brief Information log level. Contains important events that are not errors.
/// @sa CUBOS_LOG_LEVEL
#define CUBOS_LOG_LEVEL_INFO 2

/// @brief Warn log level. Contains events that are not errors, but which are unexpected and may be
/// problematic.
/// @sa CUBOS_LOG_LEVEL
#define CUBOS_LOG_LEVEL_WARN 3

/// @brief Error log level. Contains errors which are recoverable from.
/// @sa CUBOS_LOG_LEVEL
#define CUBOS_LOG_LEVEL_ERROR 4

/// @brief Critical log level, highest log level. Contains errors which are unrecoverable from.
/// @sa CUBOS_LOG_LEVEL
#define CUBOS_LOG_LEVEL_CRITICAL 5

/// @brief Off log level, disables all logging.
/// @sa CUBOS_LOG_LEVEL
#define CUBOS_LOG_LEVEL_OFF 6

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
#define CUBOS_LOG_LEVEL CUBOS_LOG_LEVEL_TRACE
#else
#define CUBOS_LOG_LEVEL CUBOS_LOG_LEVEL_INFO
#endif
#endif

/// @def CUBOS_TRACE
/// @brief Used for logging very verbose information.
/// @param ... Format string and arguments.
/// @see CUBOS_LOG_LEVEL_TRACE

#if CUBOS_LOG_LEVEL <= CUBOS_LOG_LEVEL_TRACE
#define CUBOS_TRACE(...) CUBOS_LOG(Trace, __VA_ARGS__)
#else
#define CUBOS_TRACE(...)                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (false)
#endif

/// @def CUBOS_DEBUG
/// @brief Used for logging information which is useful for debugging but not necessary in release
/// builds.
/// @param ... Format string and arguments.
/// @see CUBOS_LOG_LEVEL_DEBUG

#if CUBOS_LOG_LEVEL <= CUBOS_LOG_LEVEL_DEBUG
#define CUBOS_DEBUG(...) CUBOS_LOG(Debug, __VA_ARGS__)
#else
#define CUBOS_DEBUG(...)                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (false)
#endif

/// @def CUBOS_INFO
/// @brief Used for logging information which is useful in release builds.
/// @param ... Format string and arguments.
/// @see CUBOS_LOG_LEVEL_INFO

#if CUBOS_LOG_LEVEL <= CUBOS_LOG_LEVEL_INFO
#define CUBOS_INFO(...) CUBOS_LOG(Info, __VA_ARGS__)
#else
#define CUBOS_INFO(...)                                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (false)
#endif

/// @def CUBOS_WARN
/// @brief Used for logging unexpected events.
/// @param ... Format string and arguments.
/// @see CUBOS_LOG_LEVEL_WARN

#if CUBOS_LOG_LEVEL <= CUBOS_LOG_LEVEL_WARN
#define CUBOS_WARN(...) CUBOS_LOG(Warn, __VA_ARGS__)
#else
#define CUBOS_WARN(...)                                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (false)
#endif

/// @def CUBOS_ERROR
/// @brief Used for logging recoverable errors.
/// @param ... Format string and arguments.
/// @see CUBOS_LOG_LEVEL_ERROR

#if CUBOS_LOG_LEVEL <= CUBOS_LOG_LEVEL_ERROR
#define CUBOS_ERROR(...) CUBOS_LOG(Error, __VA_ARGS__)
#else
#define CUBOS_ERROR(...)                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (false)
#endif

/// @def CUBOS_CRITICAL
/// @brief Used for logging unrecoverable errors.
/// @param ... Format string and arguments.
/// @see CUBOS_LOG_LEVEL_CRITICAL

#if CUBOS_LOG_LEVEL <= CUBOS_LOG_LEVEL_CRITICAL
#define CUBOS_CRITICAL(...) CUBOS_LOG(Critical, __VA_ARGS__)
#else
#define CUBOS_CRITICAL(...)                                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (false)
#endif

/// @brief Used for logging messages.
/// @param level Log level.
/// @param ... Format string and arguments.
/// @see CUBOS_LOG_LEVEL_CRITICAL
#define CUBOS_LOG(level, ...)                                                                                          \
    ::cubos::core::tel::Logger::writeFormat(                                                                           \
        ::cubos::core::tel::Logger::Level::level,                                                                      \
        ::cubos::core::tel::Logger::Location{.function = __func__, .file = __FILE__, .line = __LINE__}, __VA_ARGS__)

/// @brief Aborts a program, optionally printing a critical error message.
/// @param ... Optional format string and arguments.
#define CUBOS_FAIL(...)                                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        if constexpr (sizeof(#__VA_ARGS__) > 1)                                                                        \
        {                                                                                                              \
            CUBOS_CRITICAL("" __VA_ARGS__);                                                                            \
        }                                                                                                              \
        ::cubos::core::abort();                                                                                        \
    } while (false)

/// @brief Marks a code path as supposedly unreachable. Aborts the program when reached.
/// @param ... Optional format string and arguments.
#define CUBOS_UNREACHABLE(...)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        CUBOS_CRITICAL("Reached unreachable code");                                                                    \
        CUBOS_FAIL(__VA_ARGS__);                                                                                       \
    } while (false)

/// @brief Marks a code path as unfinished. Aborts the program when reached.
/// @param ... Optional format string and arguments.
#define CUBOS_TODO(...)                                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        CUBOS_CRITICAL("Reached unfinished code");                                                                     \
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
            CUBOS_CRITICAL("Assertion " #cond " failed");                                                              \
            CUBOS_FAIL(__VA_ARGS__);                                                                                   \
        }                                                                                                              \
    } while (false)

/// @brief Asserts that an implication is true, aborting the program if it is not.
/// @param cond Implication condition.
/// @param cons Implication consequence.
/// @param ... Optional format string and arguments.
#define CUBOS_ASSERT_IMP(cond, cons, ...)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((cond) && !(cons))                                                                                         \
        {                                                                                                              \
            CUBOS_CRITICAL("Assertion " #cond " => " #cons " failed");                                                 \
            CUBOS_FAIL(__VA_ARGS__);                                                                                   \
        }                                                                                                              \
    } while (false)

/// @def CUBOS_DEBUG_ASSERT
/// @brief In debug builds asserts that a condition is true, aborting the program if it is not.
/// @param cond Condition to assert.
/// @param ... Optional format string and arguments.

#ifndef NDEBUG
#define CUBOS_DEBUG_ASSERT(cond, ...) CUBOS_ASSERT(cond)
#else
#define CUBOS_DEBUG_ASSERT(cond, ...)                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (false)
#endif

/// @}

namespace cubos::core::tel
{
    /// @brief Aborts the program and prints a pretty stack trace.
    /// @ingroup core
    [[noreturn]] void abort();

    /// @brief Singleton which holds the logging state.
    /// @ingroup core-tel
    class CUBOS_CORE_API Logger final
    {
    public:
        /// @brief Represents a logging level.
        enum class Level
        {
            /// @copybrief CUBOS_LOG_LEVEL_TRACE
            Trace = CUBOS_LOG_LEVEL_TRACE,

            /// @copybrief CUBOS_LOG_LEVEL_DEBUG
            Debug = CUBOS_LOG_LEVEL_DEBUG,

            /// @copybrief CUBOS_LOG_LEVEL_INFO
            Info = CUBOS_LOG_LEVEL_INFO,

            /// @copybrief CUBOS_LOG_LEVEL_WARN
            Warn = CUBOS_LOG_LEVEL_WARN,

            /// @copybrief CUBOS_LOG_LEVEL_ERROR
            Error = CUBOS_LOG_LEVEL_ERROR,

            /// @copybrief CUBOS_LOG_LEVEL_CRITICAL
            Critical = CUBOS_LOG_LEVEL_CRITICAL,

            /// @copybrief CUBOS_LOG_LEVEL_OFF
            Off = CUBOS_LOG_LEVEL_OFF,
        };

        /// @brief Identifies a location in the code.
        struct Location
        {
            std::string function; ///< Function name.
            std::string file;     ///< File name.
            int line;             ///< Line number.

            /// @brief Returns a string representation for the location.
            /// @return String representation.
            std::string string() const;
        };

        /// @brief A timestamp used to identify when a logging message was written.
        struct Timestamp
        {
            int64_t timestamp; ///< Milliseconds since the UNIX epoch.

            /// @brief Returns a timestamp with the current time.
            /// @return Timestamp.
            static Timestamp now();

            /// @brief Returns a string representation for the timestamp, containing hours,
            /// minutes, seconds and milliseconds.
            /// @return String representation.
            std::string string() const;
        };

        /// @brief Data created by a call to @ref log.
        struct Entry
        {
            Level level;         ///< Level.
            Timestamp timestamp; ///< Timestamp;
            Location location;   ///< Code location which originated the log message.
            std::string message; ///< Message.
        };

        /// @brief Deleted constructor.
        Logger() = delete;

        /// @brief Sets the log level.
        ///
        /// If @ref CUBOS_LOG_LEVEL is higher than the set level, entries below it are still not
        /// registered.
        ///
        /// @param level New logging level.
        static void level(Level level);

        /// @brief Gets the current log level.
        /// @return Log level.
        static Level level();

        /// @brief Sets a file path where logs will be saved.
        /// @note Previous logs are also dumped to the file.
        /// @param filePath Path to file in the virtual file system.
        /// @return Whether the file could be opened for logging.
        static bool logToFile(const std::string& filePath);

        /// @brief Mounts a standard archive on the `/logs/` directory, and calls `logToFile` to a timestamped log file
        /// on that directory.
        /// @note Previous logs are also dumped to the file.
        /// @return Whether the file could be mounted and opened for logging.
        static bool logToFile();

        /// @brief Creates a new entry in the logs, as long as the log level is high enough.
        /// @param level Log level.
        /// @param location Code location.
        /// @param message Log message.
        static void write(Level level, Location location, std::string message);

        /// @brief Wrapper for @ref write() with message formatting.
        /// @tparam TArgs Message format argument types.
        /// @param level Log level.
        /// @param location Code location.
        /// @param format Message format string.
        /// @param args Message format arguments.
        template <typename... TArgs>
        static void writeFormat(Level level, Location location, const char* format, TArgs... args)
        {
            memory::BufferStream stream{};
            if constexpr (sizeof...(TArgs) == 0)
            {
                stream.print(format);
            }
            else
            {
                ([&]() { format = Logger::streamFormat(stream, format, reflection::reflect<TArgs>(), &args); }(), ...);
            }
            Logger::write(level, std::move(location), stream.string());
        }

        /// @brief Reads a log entry, if there's a new one.
        /// @param[out] cursor Index of the entry to be read. Automatically increased.
        /// @param[out] entry Entry to read into.
        /// @return Whether an entry was red.
        static bool read(std::size_t& cursor, Entry& entry);

    private:
        /// @brief Writes the given format string to the given stream, with a single argument type.
        /// Stops if a second argument is found, and returns a pointer to its location.
        /// @param stream Stream to write to.
        /// @param format Format string.
        /// @param type Argument type.
        /// @param value Argument value.
        /// @return Pointer to second argument location, or to null terminating character, if over.
        static const char* streamFormat(memory::Stream& stream, const char* format, const reflection::Type& type,
                                        const void* value);
    };
} // namespace cubos::core::tel

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, cubos::core::tel::Logger::Level);
