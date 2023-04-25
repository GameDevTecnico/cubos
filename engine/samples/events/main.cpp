#include <cubos/engine/cubos.hpp>
#include <cubos/core/ecs/event_writer.hpp>
#include <cubos/core/ecs/event_reader.hpp>

using namespace cubos::core;

struct MyEvent
{
    int value;
};

struct State
{
    int step;
};

int main()
{
    cubos::engine::Cubos cubos;
    cubos.addResource<State>(State{.step = 0});
    cubos.addEvent<MyEvent>();
    cubos.startupSystem([](cubos::engine::ShouldQuit& quit) { quit.value = false; });
    cubos
        .system([](ecs::EventReader<MyEvent> reader) {
            for (const auto& event : reader)
            {
                CUBOS_INFO("A read {}", event.value);
            }
        })
        .tagged("A")
        .beforeTag("B");
    cubos
        .system([](ecs::EventWriter<MyEvent> writer, State& state, cubos::engine::ShouldQuit& quit) {
            state.step += 1;
            if (state.step == 1) // Write 1 2 3 on first run.
            {
                writer.push({1});
                writer.push({2});
                writer.push({3});
                CUBOS_INFO("B wrote 1 2 3");
            }
            else if (state.step == 2)
            {
                quit.value = true; // Stop the loop.
                writer.push({4});
                writer.push({5});
                writer.push({6});
                CUBOS_INFO("B wrote 4 5 6");
            }
        })
        .tagged("B");
    cubos
        .system([](ecs::EventReader<MyEvent> reader) {
            for (const auto& event : reader)
            {
                CUBOS_INFO("C read {}", event.value);
            }
        })
        .tagged("C")
        .afterTag("B");
    cubos
        .system([](ecs::EventReader<MyEvent> reader) {
            for (const auto& event : reader)
            {
                CUBOS_INFO("D read {}", event.value);
            }
        })
        .tagged("D")
        .afterTag("C");

    // Should print:
    // B wrote 1 2 3
    // C read 1
    // C read 2
    // C read 3
    // D read 1
    // D read 2
    // D read 3
    // A read 1
    // A read 2
    // A read 3
    // B wrote 4 5 6
    // C read 4
    // C read 5
    // C read 6
    // D read 4
    // D read 5
    // D read 6
    cubos.run();
}
