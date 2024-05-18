# Metrics {#examples-core-metrics}

@brief Using the metrics system.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/metrics).

Registering metrics and profiling in **CUBOS.** can be done through the metric macros defined in @ref core/metrics.hpp and the @ref core::Metrics class.
In order to enable these features, compile with `-DENABLE_PROFILING`.

@snippet metrics/main.cpp Import metrics

The following example demonstrates a simple usage of the @ref core::Metrics class, simulating a random function that takes X milliseconds to finish, where X is randomly generated, and also registering metrics values.

This is the main loop:

@snippet metrics/main.cpp Simulate profiling loop

Which calls these utility functions:

@snippet metrics/main.cpp Simulate profiling

@snippet metrics/main.cpp Generate random value

We can now get the existing metrics and their values:

@snippet metrics/main.cpp Get metrics and values

Output:

```
[11:34:13.824] [main.cpp:51 main] debug: Found new metric: "profiling:/home/cubos/core/samples/metrics/main.cpp:simulateFrame:27"
[11:34:13.824] [main.cpp:56 main] debug: "profiling:/home/cubos/core/samples/metrics/main.cpp:simulateFrame:27" : 103.0721
[11:34:13.824] [main.cpp:56 main] debug: "profiling:/home/cubos/core/samples/metrics/main.cpp:simulateFrame:27" : 469.1480
[11:34:13.825] [main.cpp:56 main] debug: "profiling:/home/cubos/core/samples/metrics/main.cpp:simulateFrame:27" : 379.0656
[11:34:13.825] [main.cpp:56 main] debug: "profiling:/home/cubos/core/samples/metrics/main.cpp:simulateFrame:27" : 388.1754
[11:34:13.825] [main.cpp:56 main] debug: "profiling:/home/cubos/core/samples/metrics/main.cpp:simulateFrame:27" : 37.1140
[11:34:13.825] [main.cpp:56 main] debug: "profiling:/home/cubos/core/samples/metrics/main.cpp:simulateFrame:27" : 14.0966
[11:34:13.825] [main.cpp:56 main] debug: "profiling:/home/cubos/core/samples/metrics/main.cpp:simulateFrame:27" : 8.1281
[11:34:13.825] [main.cpp:56 main] debug: "profiling:/home/cubos/core/samples/metrics/main.cpp:simulateFrame:27" : 464.1636
[11:34:13.826] [main.cpp:56 main] debug: "profiling:/home/cubos/core/samples/metrics/main.cpp:simulateFrame:27" : 313.1553
[11:34:13.826] [main.cpp:56 main] debug: "profiling:/home/cubos/core/samples/metrics/main.cpp:simulateFrame:27" : 57.0801
[11:34:13.826] [main.cpp:51 main] debug: Found new metric: "i"
[11:34:13.826] [main.cpp:56 main] debug: "i" : 0.0000
[11:34:13.827] [main.cpp:56 main] debug: "i" : 1.0000
[11:34:13.827] [main.cpp:56 main] debug: "i" : 2.0000
[11:34:13.827] [main.cpp:56 main] debug: "i" : 3.0000
[11:34:13.827] [main.cpp:56 main] debug: "i" : 4.0000
[11:34:13.827] [main.cpp:56 main] debug: "i" : 5.0000
[11:34:13.828] [main.cpp:56 main] debug: "i" : 6.0000
[11:34:13.828] [main.cpp:56 main] debug: "i" : 7.0000
[11:34:13.828] [main.cpp:56 main] debug: "i" : 8.0000
[11:34:13.828] [main.cpp:56 main] debug: "i" : 9.0000
```