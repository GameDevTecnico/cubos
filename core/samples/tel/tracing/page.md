# Tracing {#examples-core-tel-tracing}

@brief Using the tracing system.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/tracing).

Tracing in **CUBOS** can be done through the tracing macros defined in @ref core/tel/tracing.hpp and the @ref core::tel::SpanManager class. 

@snippet tracing/main.cpp Logging and tracing include

The following example demonstrates a simple usage of the tracing macros and @ref core::tel::SpanManager class, utilizing spans to track execution flow and log messages.

@snippet tracing/main.cpp Set level

The tracing level is shared with logger's one, via @ref core::tel::level function.

Using the macros is simple as:

@snippet tracing/main.cpp Using macros

You can also do manual span management:

@snippet tracing/main.cpp Manually

We can observe the output of the tracing as follows:

```s
[16:03:31.966] [main.cpp:20 main] [thread11740] info: hello from root span!
[16:03:31.967] [main.cpp:26 main] [thread11740:main_span] info: hello!
[16:03:31.967] [main.cpp:29 main] [thread11740:main_span:other_scope] info: hello again!
[16:03:31.968] [main.cpp:34 main] [thread11740:main_span:other_scope:manual_span] info: entered a manual span
[16:03:31.969] [main.cpp:37 main] [thread11740:main_span:other_scope] info: after exit manual span
```

