#include <gtest/gtest.h>
#include <cubos/core/log.hpp>
#include <cubos/core/ecs/event_pipe.hpp>
#include <cubos/core/ecs/event_writer.hpp>
#include <cubos/core/ecs/event_reader.hpp>

using namespace cubos::core::ecs;
using namespace cubos::core::memory;

TEST(Cubos_Core_Event_System, Event_System_Simple_Test)
{
    auto pipe = EventPipe<int>();
    auto writer = EventWriter<int>(pipe);
    auto reader = EventReader<int, void>(pipe);

    std::size_t size = 0;
    for (auto x : reader)
    {
        size++;
    }
    EXPECT_EQ(size, 0);

    writer.push(3);
    size = 0;
    for (auto x : reader)
    {
        size++;
    }
    EXPECT_EQ(size, 1);

    pipe.clear();
    size = 0;
    for (auto x : reader)
    {
        size++;
    }
    EXPECT_EQ(size, 0);
}