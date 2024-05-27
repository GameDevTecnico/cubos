# Logging {#examples-core-logging}

@brief Using the logging system.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/logging).

Logging in **Cubos** is done through the logging macros defined in @ref core/log.hpp.

@snippet logging/main.cpp Logging include

There are six logging levels, each with its own macro.
In order of increasing severity, they are:

@snippet logging/main.cpp Logging macros

The minimum registered log level is set with @ref cubos::core::Logger::level "Logger::level". On
this sample, to demonstrate @ref CUBOS_TRACE, we set it to @ref cubos::core::Logger::Level::Trace
"Trace":

@snippet logging/main.cpp Set logging level

@note By default, on debug builds all logging calls are compiled, while on release builds only
messages with severity level @ref CUBOS_LOG_LEVEL_INFO or higher are compiled. This can be changed
by defining @ref CUBOS_LOG_LEVEL to the desired level.

These macros can also take arguments, which can be of any reflectable type.

@snippet logging/main.cpp Logging macros with arguments

To print external types, such as `glm` math types, `STL` types (`std::string`, ...) or primitives
(`int`, ...), you'll have to include their respective headers in the @ref core/reflection/external
directory. Notice that although we aren't printing C-strings directly, we must still include them
as our `std::unordered_map` contains `const char*`s.

@snippet logging/main.cpp External reflection includes
