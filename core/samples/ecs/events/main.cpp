#include <cubos/core/ecs/system/arguments/event/pipe.hpp>
#include <cubos/core/ecs/system/arguments/event/reader.hpp>
#include <cubos/core/ecs/system/arguments/event/writer.hpp>
#include <cubos/core/memory/stream.hpp>
#include <cubos/core/tel/logging.hpp>

using namespace cubos::core::ecs;
using namespace cubos::core::memory;

int main()
{
    struct MyEvent
    {
        enum Mask
        {
            KeyEvent = 1 << 0,   // 0001
            MouseEvent = 1 << 1, // 0010
            WheelEvent = 1 << 2, // 0100
            ALL = ~0U,           // 1111
        };

        int data; // random data member
    };

    auto pipe = EventPipe<MyEvent>();
    auto writer = EventWriter<MyEvent>(pipe);

    writer.push(MyEvent{.data = 4}, MyEvent::Mask::KeyEvent);
    writer.push(MyEvent{.data = 1}, MyEvent::Mask::WheelEvent);
    writer.push(MyEvent{.data = 2}, MyEvent::Mask::MouseEvent);
    writer.push(MyEvent{.data = 6}, MyEvent::Mask::MouseEvent);
    writer.push(MyEvent{.data = 15});
    writer.push(MyEvent{.data = 11});

    std::size_t index = 0;

    Stream::stdOut.printf("\n\n### mouse events using .read():");
    while (true)
    {
        auto mouseReader = EventReader<MyEvent, MyEvent::Mask::MouseEvent>(pipe, index);
        const auto* it = mouseReader.read();
        if (it == nullptr)
        {
            break;
        }
        Stream::stdOut.printf(" {} , ", it->data);
    }

    // range based

    index = 0;
    auto mouseReader = EventReader<MyEvent, MyEvent::Mask::MouseEvent>(pipe, index);
    Stream::stdOut.printf("\n\n### mouse events:");
    const auto* x = mouseReader.read(); // already read one event, the loop will only loop once
    Stream::stdOut.printf("{} , ", x->data);
    for (const auto& it : mouseReader)
    {
        Stream::stdOut.printf(" {} , ", it.data); // 6
    }

    index = 0;
    Stream::stdOut.printf("\n\n### wheel + key events:");
    for (const auto& it : EventReader<MyEvent, MyEvent::Mask::KeyEvent | MyEvent::Mask::WheelEvent>(pipe, index))
    {
        Stream::stdOut.printf(" {} , ", it.data);
    }

    index = 0;
    Stream::stdOut.printf("\n\n### all masked events:");
    for (const auto& it : EventReader<MyEvent, MyEvent::Mask::ALL>(pipe, index))
    {
        Stream::stdOut.printf(" {} , ", it.data);
    }

    index = 0;
    Stream::stdOut.printf("\n\n### all events (no M parameter):");
    for (const auto& it : EventReader<MyEvent>(pipe, index))
    {
        Stream::stdOut.printf(" {} , ", it.data);
    }

    Stream::stdOut.printf("\n");
}
