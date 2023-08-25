# Logging {#examples-core-logging}

@brief Using the logging system.

Logging in **CUBOS.** is done through the logging macros defined in @ref core/log.hpp.

@dontinclude logging.cpp
@snippet logging.cpp Logging include

Before any logging is done, the logger must be initialized.
This usually happens right at the beginning of the program.

@snippet logging.cpp Logger initialization

There are six logging levels, each with its own macro.
In order of increasing severity, they are:

@snippet logging.cpp Logging macros

These macros can also take arguments:

@snippet logging.cpp Logging macros with arguments

@note By default, on debug builds all messages are logged, while on release builds only messages with
severity level @ref CUBOS_LOG_LEVEL_INFO or higher are logged. This can be changed by defining
@ref CUBOS_LOG_LEVEL to the desired level.

Serializable types can also be logged, using @ref cubos::core::data::Debug.

@snippet logging.cpp Debug wrapper include

By wrapping the value in @ref cubos::core::data::Debug, the type is serialized using the
@ref cubos::core::data::DebugSerializer, and the result is logged.

@snippet logging.cpp Debug wrapper usage