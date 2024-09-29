#include "debugger.hpp"
#include <condition_variable>
#include <mutex>
#include <thread>

#include <cubos/core/data/des/binary.hpp>
#include <cubos/core/data/ser/binary.hpp>
#include <cubos/core/ecs/cubos.hpp>
#include <cubos/core/memory/function.hpp>
#include <cubos/core/tel/logging.hpp>

using namespace cubos::core;

namespace
{
    struct Command
    {
        memory::Function<void()> action;
        bool shouldDisconnect = false;
        bool shouldCloseApplication = false;

        static Command makeAction(memory::Function<void()> action)
        {
            return {std::move(action)};
        }

        static Command makeDisconnect()
        {
            return {nullptr, true, false};
        }

        static Command makeCloseApplication()
        {
            return {nullptr, true, true};
        }
    };

    struct State
    {
        std::mutex mutex;
        std::condition_variable onPush;
        std::condition_variable onPop;
        memory::Opt<Command> command;
    };

    void controller(State& state, memory::Stream& stream, ecs::Cubos& cubos)
    {
        bool shouldCloseApplication = false;

        // Setup serializers.
        data::BinarySerializer ser{stream};
        data::BinaryDeserializer des{stream};

        while (true)
        {
            std::string command;
            if (!des.read(command))
            {
                CUBOS_ERROR("Failed to read command from debugger client, closing connection");
                break;
            }

            // Lock the runner so that we can update the command safely.
            CUBOS_INFO("Received command {} from debugger client", command);
            std::unique_lock lock{state.mutex};
            state.onPop.wait(lock, [&] { return !state.command.contains(); });

            if (command == "run")
            {
                // Run the application continuously, until some other command is received.
                state.command.replace(Command::makeAction([&] {
                    if (!cubos.started())
                    {
                        cubos.start();
                    }

                    if (!cubos.shouldQuit())
                    {
                        cubos.update();
                    }
                }));
            }
            else if (command == "pause")
            {
                // Simply clear the action so that the runner does not do anything.
                state.command.replace(Command::makeAction(nullptr));
            }
            else if (command == "update")
            {
                // Update the application a certain number of times.
                std::size_t count;
                if (!des.read(count))
                {
                    CUBOS_ERROR("Failed to read update count from debugger client, closing connection");
                    break;
                }

                state.command.replace(Command::makeAction([&, count]() mutable {
                    if (!cubos.started())
                    {
                        cubos.start();
                    }

                    if (cubos.shouldQuit())
                    {
                        count = 0;
                    }

                    if (count > 0)
                    {
                        cubos.update();
                        count--;
                    }
                }));
            }
            else if (command == "close")
            {
                shouldCloseApplication = true;
                break;
            }
            else if (command == "disconnect")
            {
                break;
            }
            else
            {
                CUBOS_ERROR("Unknown command {} from debugger client, closing connection", command);
                break;
            }

            state.onPush.notify_one();
        }

        // Notify the application to disconnect.
        {
            std::unique_lock lock{state.mutex};
            state.onPop.wait(lock, [&] { return !state.command.contains(); });
            if (shouldCloseApplication)
            {
                state.command.replace(Command::makeCloseApplication());
            }
            else
            {
                state.command.replace(Command::makeDisconnect());
            }
            state.onPush.notify_one();
        }
    }
} // namespace

bool cubos::core::ecs::debugger(memory::Stream& stream, Cubos& cubos)
{
    auto typeConnection = cubos.typeServer()->connect(stream);
    if (!typeConnection.contains())
    {
        CUBOS_ERROR("Failed to share reflection data with client, closing connection");
        return true;
    }

    CUBOS_INFO("Successfully shared reflection data with client, waiting for commands");

    // Launch the thread which will control the application.
    State state{};
    std::thread controllerThread{controller, std::ref(state), std::ref(stream), std::ref(cubos)};

    memory::Function<void()> action;
    bool shouldDisconnect = false;
    bool shouldContinueRunning = true;
    while (!shouldDisconnect)
    {
        {
            std::unique_lock lock{state.mutex};
            state.onPush.wait(lock, [&] { return state.command.contains() || action != nullptr; });

            if (state.command.contains())
            {
                state.onPop.notify_one();

                action = std::move(state.command->action);
                shouldDisconnect = state.command->shouldDisconnect;
                shouldContinueRunning = !state.command->shouldCloseApplication;
                state.command.reset();
            }
        }

        if (action != nullptr)
        {
            action();
        }
    }

    controllerThread.join();

    return shouldContinueRunning && !cubos.shouldQuit();
}
