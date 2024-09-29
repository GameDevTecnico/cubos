# Metrics {#examples-core-tel-metrics}

@brief Using the metrics system.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/metrics).

Registering metrics and profiling in **Cubos** can be done through the metric macros defined in @ref core/tel/metrics.hpp and the @ref core::tel::Metrics class.
In order to enable these features, compile with `-DENABLE_PROFILING`.

@snippet metrics/main.cpp Import metrics

The following example demonstrates a simple usage of the @ref core::tel::Metrics class, simulating a random function that takes X milliseconds to finish, where X is randomly generated, and also registering metrics values.

This is the main loop:

@snippet metrics/main.cpp Simulate profiling loop

Which calls these utility functions:

@snippet metrics/main.cpp Simulate profiling

@snippet metrics/main.cpp Generate random value

We can now get the existing metrics and their values:

@snippet metrics/main.cpp Get metrics and values

Output:

```
[17:21:59.677] [main.cpp:53 main] [thread21496] debug: Found new metric: "thread21496:simulateFrame:begin"
[17:21:59.679] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:begin" : 5.0244E+8
[17:21:59.680] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:begin" : 5.0244E+8
[17:21:59.681] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:begin" : 5.0244E+8
[17:21:59.681] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:begin" : 5.0244E+8
[17:21:59.682] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:begin" : 5.0244E+8
[17:21:59.683] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:begin" : 5.0244E+8
[17:21:59.684] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:begin" : 5.0244E+8
[17:21:59.685] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:begin" : 5.0244E+8
[17:21:59.686] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:begin" : 5.0244E+8
[17:21:59.687] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:begin" : 5.0244E+8
[17:21:59.687] [main.cpp:53 main] [thread21496] debug: Found new metric: "thread21496:simulateFrame:end"
[17:21:59.688] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:end" : 5.0244E+8
[17:21:59.689] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:end" : 5.0244E+8
[17:21:59.690] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:end" : 5.0244E+8
[17:21:59.691] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:end" : 5.0244E+8
[17:21:59.693] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:end" : 5.0244E+8
[17:21:59.694] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:end" : 5.0244E+8
[17:21:59.695] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:end" : 5.0244E+8
[17:21:59.696] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:end" : 5.0244E+8
[17:21:59.697] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:end" : 5.0244E+8
[17:21:59.698] [main.cpp:58 main] [thread21496] debug: "thread21496:simulateFrame:end" : 5.0244E+8
[17:21:59.699] [main.cpp:53 main] [thread21496] debug: Found new metric: "thread21496:i"
[17:21:59.700] [main.cpp:58 main] [thread21496] debug: "thread21496:i" : 0.0000
[17:21:59.701] [main.cpp:58 main] [thread21496] debug: "thread21496:i" : 1.0000
[17:21:59.701] [main.cpp:58 main] [thread21496] debug: "thread21496:i" : 2.0000
[17:21:59.702] [main.cpp:58 main] [thread21496] debug: "thread21496:i" : 3.0000
[17:21:59.703] [main.cpp:58 main] [thread21496] debug: "thread21496:i" : 4.0000
[17:21:59.704] [main.cpp:58 main] [thread21496] debug: "thread21496:i" : 5.0000
[17:21:59.705] [main.cpp:58 main] [thread21496] debug: "thread21496:i" : 6.0000
[17:21:59.706] [main.cpp:58 main] [thread21496] debug: "thread21496:i" : 7.0000
[17:21:59.707] [main.cpp:58 main] [thread21496] debug: "thread21496:i" : 8.0000
[17:21:59.708] [main.cpp:58 main] [thread21496] debug: "thread21496:i" : 9.0000
```