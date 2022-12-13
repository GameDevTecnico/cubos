#include <cubos/core/log.hpp>
#include <cubos/core/ecs/event_pipe.hpp>
#include <cubos/core/ecs/event_writer.hpp>
#include <cubos/core/ecs/event_reader.hpp>

using namespace cubos::core::ecs;
using namespace cubos::core::memory;

int main()
{
    cubos::core::initializeLogger();

    auto pipe = EventPipe<int>();
    auto writer = EventWriter<int>(pipe);
    writer.push(3);
    auto reader = EventReader<int, void>(pipe);

    Stream::stdOut.printf("### events: \n");
    for (auto event : reader)
    {
        Stream::stdOut.printf("# {} \n", event);
    }
    pipe.clear();

    Stream::stdOut.printf("### after clearing: \n");
    for (auto event : reader)
    {
        Stream::stdOut.printf("# {} \n", event);
    }
}