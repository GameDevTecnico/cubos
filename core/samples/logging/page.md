# Logging {#examples-core-logging}

@brief Using the logging system.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/logging).

Logging in **Cubos** is done through the logging macros defined in @ref core/tel/logging.hpp.

@snippet logging/main.cpp Logging include

There are six logging levels, each with its own macro.
In order of increasing severity, they are:

@snippet logging/main.cpp Logging macros

The minimum registered log level is set with @ref cubos::core::tel::Logger::level "Logger::level". On
this sample, to demonstrate @ref CUBOS_TRACE, we set it to @ref cubos::core::tel::Logger::Level::Trace
"Trace":

@snippet logging/main.cpp Set logging level

The logger also supports logging to a file, which will by default be named to `cubos_<timestamp>.log`.

@snippet logging/main.cpp Log to file

@note The logger will by default store logs inside `/logs`.

Or, if you want, you can set the file manually:

@snippet logging/main.cpp Set logging file

@note The logger will **always** dump previous logs to the file.

```sh
$ cat /home/cubos/build/logs/cubos_15:25:30.191.log
[15:25:30.192] [file.cpp:124 mount] info: Mounted archive at ""/"logs"
[15:25:30.192] [file.cpp:322 create] trace: Created "file" "/logs/cubos_15:25:30.191"
[15:25:30.192] [main.cpp:29 main] trace: Trace message
[15:25:30.192] [main.cpp:30 main] debug: Debug message
[15:25:30.192] [main.cpp:31 main] info: Info message
[15:25:30.192] [main.cpp:32 main] warn: Warning message
[15:25:30.193] [main.cpp:33 main] error: Error message
[15:25:30.193] [main.cpp:34 main] critical: Critical message
[15:25:30.193] [file.cpp:85 mount] error: Could not mount archive at "/logs"/"": "/logs" is already part of an archive
[15:25:30.193] [main.cpp:42 main] info: An integer: 1
[15:25:30.193] [main.cpp:43 main] info: A glm::vec3: (x: 0.0000, y: 1.0000, z: 2.0000)
[15:25:30.194] [main.cpp:44 main] info: An std::unordered_map: {2: "two", 1: "one"}
[15:25:30.194] [log.cpp:248 streamFormat] warn: You tried to print a type ("unnamed102322537939356") which doesn't implement reflection. Did you forget to include its reflection definition?
[15:25:30.194] [main.cpp:45 main] info: A type without reflection: (no reflection)
```

```sh
$ cat /home/cubos/build/logs/sample_logs.txt 
[15:23:03.737] [file.cpp:124 mount] info: Mounted archive at ""/"logs"
[15:23:03.737] [main.cpp:29 main] trace: Trace message
[15:23:03.737] [main.cpp:30 main] debug: Debug message
[15:23:03.737] [main.cpp:31 main] info: Info message
[15:23:03.737] [main.cpp:32 main] warn: Warning message
[15:23:03.737] [main.cpp:33 main] error: Error message
[15:23:03.738] [main.cpp:34 main] critical: Critical message
[15:23:03.738] [main.cpp:38 main] info: An integer: 1
[15:23:03.738] [main.cpp:39 main] info: A glm::vec3: (x: 0.0000, y: 1.0000, z: 2.0000)
[15:23:03.738] [main.cpp:40 main] info: An std::unordered_map: {2: "two", 1: "one"}
[15:23:03.738] [log.cpp:248 streamFormat] warn: You tried to print a type ("unnamed108664291041692") which doesn't implement reflection. Did you forget to include its reflection definition?
[15:23:03.738] [main.cpp:41 main] info: A type without reflection: (no reflection)
t reflection: (no reflection)
```

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
