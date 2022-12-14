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
    auto reader = EventReader<int, 0>(pipe);

    std::size_t size = 0;
    for (auto& x : reader)
    {
        size++;
    }
    EXPECT_EQ(size, 0);

    writer.push(3);
    for (size = 0; auto& x : reader)
    {
        size++;
    }
    EXPECT_EQ(size, 1);

    writer.push(3, 5);
    for (size = 0; auto& x : reader)
    {
        size++;
    }
    EXPECT_EQ(size, 1); // still one because we added a event "3" with mask 5

    writer.push(2);
    for (size = 0; auto& x : reader)
    {
        size++;
    }
    EXPECT_EQ(size, 2); // still one because we added a event "3" with mask 5

    pipe.clear(); // clears only (3, 0) and (2, 0), because the reader is only on "0" mask events
    for (size = 0; auto& x : reader)
    {
        size++;
    }
    EXPECT_EQ(size, 1); // (3,5) event was not read yet
}

TEST(Cubos_Core_Event_System, Event_Pipe_Writer_Simple_Test)
{
    auto pipe = EventPipe<int>();
    auto writer = EventWriter<int>(pipe);

    EXPECT_EQ(pipe.size(), 0);

    writer.push(3);
    EXPECT_EQ(pipe.size(), 1);

    writer.push(3, 5);
    EXPECT_EQ(pipe.size(), 2);

    pipe.clear();
    EXPECT_EQ(pipe.size(), 0);
}

TEST(Cubos_Core_Event_System, Event_System_Masking_Test)
{
    struct MyEvent
    {
        enum Mask
        {
            KEY_EVENT = 1 << 0,   // 0001
            MOUSE_EVENT = 1 << 1, // 0010
            WHEEL_EVENT = 1 << 2, // 0100
            ALL = (1 << 3) - 1,   // 0111
        };

        int data; // random data member
    };

    auto pipe = EventPipe<MyEvent>();
    auto writer = EventWriter<MyEvent>(pipe);

    writer.push(MyEvent{.data = 1}, MyEvent::Mask::KEY_EVENT);
    writer.push(MyEvent{.data = 2}, MyEvent::Mask::WHEEL_EVENT);
    writer.push(MyEvent{.data = 3}, MyEvent::Mask::MOUSE_EVENT);
    writer.push(MyEvent{.data = 4}, MyEvent::Mask::MOUSE_EVENT);
    writer.push(MyEvent{.data = 5});
    writer.push(MyEvent{.data = 6});

    auto keyEventReader = EventReader<MyEvent, MyEvent::Mask::KEY_EVENT>(pipe);
    for (auto& ev : keyEventReader)
    {
        // should run only once
        EXPECT_EQ(ev.data, 1);
        ev.data = 1337;
        EXPECT_EQ(ev.data, 1337);
    }
    for (auto& ev : keyEventReader)
    {
        // this should NOT run, because all KEY_EVENTS from keyEventReader already got read
        EXPECT_EQ(false, true);
    }

    for (const auto& ev : EventReader<MyEvent, MyEvent::Mask::MOUSE_EVENT>(pipe))
    {
        auto possibleData = ev.data == 3 || ev.data == 4;
        EXPECT_EQ(possibleData, true);
    }

    for (const auto& ev : EventReader<MyEvent, MyEvent::Mask::KEY_EVENT | MyEvent::Mask::MOUSE_EVENT>(pipe))
    {
        auto possibleData = ev.data == 1337 || ev.data == 3 || ev.data == 4;
        EXPECT_EQ(possibleData, true);
    }

    std::size_t size = 0;
    for (const auto& ev : EventReader<MyEvent, 0>(pipe))
    {
        size++;
    }
    EXPECT_EQ(size, 2);

    size = 0;
    for (const auto& ev : EventReader<MyEvent, MyEvent::Mask::ALL>(pipe))
    {
        size++;
    }
    EXPECT_EQ(size, 4);
}