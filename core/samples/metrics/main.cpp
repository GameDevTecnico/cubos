#include <chrono>
#include <iostream>
#include <thread>

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/tel/logging.hpp>

/// [Import metrics]
#include <cubos/core/metrics.hpp>

using cubos::core::Metrics;
/// [Import metrics]

/// [Generate random value]
static int randomMs(int min, int max)
{
    int range = (min - max) + 1;
    return rand() % range + min;
}
/// [Generate random value]

/// [Simulate profiling]
static void simulateFrame(int ms)
{
    CUBOS_PROFILE();
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
/// [Simulate profiling]

int main()
{
    srand((unsigned int)time(nullptr));
    Metrics::clear();

    /// [Simulate profiling loop]
    const int numFrames = 10;
    for (int i = 0; i < numFrames; ++i)
    {
        // simulate frame by calling a function that does work..
        simulateFrame(randomMs(0, 500));

        // register some metrics, this could be FPS, entities count, ....
        CUBOS_METRIC("i", i);
    }
    /// [Simulate profiling loop]

    /// [Get metrics and values]
    std::string name;
    std::size_t seenCount = 0;
    while (Metrics::readName(name, seenCount))
    {
        CUBOS_DEBUG("Found new metric: {}", name);
        double value;
        std::size_t offset = 0;
        while (Metrics::readValue(name, value, offset))
        {
            CUBOS_DEBUG("{} : {}", name, value);
        }
    }
    /// [Get metrics and values]

    return 0;
}
