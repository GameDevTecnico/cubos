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
    writer.push(1);
    writer.push(4);
    auto reader = EventReader<int, 2>(pipe);

    printf("### events:\n");
    for (auto it = reader.begin(), end = reader.end(); it != end; ++it)
    {
        const auto i = *it;
        printf("\nevent vdata: : %d\n", i);
    }
}