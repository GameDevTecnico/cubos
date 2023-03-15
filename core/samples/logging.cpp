#include <cubos/core/log.hpp>
#include <cubos/core/memory/buffer_stream.hpp>
#include <cubos/core/data/debug_serializer.hpp>

using namespace cubos::core;

int main(void)
{
    initializeLogger();

    CUBOS_TRACE("Trace message");
    CUBOS_DEBUG("Debug message");
    CUBOS_INFO("Info message");
    CUBOS_WARN("Warning message");
    CUBOS_ERROR("Error message");
    CUBOS_CRITICAL("Critical message");

    return 0;
}
