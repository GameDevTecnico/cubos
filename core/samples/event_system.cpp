#include <cubos/core/log.hpp>
#include <cubos/core/ecs/event_pipe.hpp>
#include <cubos/core/ecs/event_writer.hpp>
#include <cubos/core/ecs/event_reader.hpp>

using namespace cubos::core::ecs;
using namespace cubos::core::memory;

int main()
{
    cubos::core::initializeLogger();

    struct MyEvent
    {
        enum Mask
        {
            KEY_EVENT = 1 << 0,   // 0001
            MOUSE_EVENT = 1 << 1, // 0010
            WHEEL_EVENT = 1 << 2, // 0100
            ALL = (1 << 3) - 1,   // 0111
        };

        char a;
        int data; // random data member
    };

    auto pipe = EventPipe<MyEvent>();
    auto writer = EventWriter<MyEvent>(pipe);

    writer.push(MyEvent{.data = 4}, MyEvent::Mask::KEY_EVENT);
    writer.push(MyEvent{.data = 1}, MyEvent::Mask::WHEEL_EVENT);
    writer.push(MyEvent{.data = 2}, MyEvent::Mask::MOUSE_EVENT);
    writer.push(MyEvent{.data = 6}, MyEvent::Mask::MOUSE_EVENT);
    writer.push(MyEvent{.data = 2}); // MyEvent::Mask::ALL ignores this, should it?
    writer.push(MyEvent{.data = 6}); // MyEvent::Mask::ALL ignores this, should it?

    printf("### mouse events using .read():\n");
    while (true)
    {
        static auto mouseReader = EventReader<MyEvent, MyEvent::Mask::MOUSE_EVENT>(pipe);
        auto it = mouseReader.read();
        if (it == std::nullopt)
        {
            break;
        }
        printf("\t ## data : %d\n", it->get().data);
    }

    // range based

    auto mouseReader = EventReader<MyEvent, MyEvent::Mask::MOUSE_EVENT>(pipe);
    printf("### mouse events:\n");
    mouseReader.read(); // already read one event, the loop will only loop ince
    for (const auto& it : mouseReader)
    {
        printf("\t ## data: : %d\n", it.data);
    }

    writer.push(MyEvent{.data = 2});

    printf("### wheel + key events:\n");
    for (const auto& it : EventReader<MyEvent, MyEvent::Mask::KEY_EVENT | MyEvent::Mask::WHEEL_EVENT>(pipe))
    {
        printf("\t ## data: : %d\n", it.data);
    }

    printf("\n### all masked events:\n");
    for (const auto& it : EventReader<MyEvent, MyEvent::Mask::ALL>(pipe))
    {
        printf("\t ## data: : %d\n", it.data);
    }

    printf("\n### all non masked events:\n");
    for (const auto& it : EventReader<MyEvent, 0>(pipe))
    {
        printf("\t ## data: : %d\n", it.data);
    }
}
