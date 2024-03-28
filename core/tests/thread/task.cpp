#include <thread>

#include <doctest/doctest.h>

#include <cubos/core/thread/task.hpp>

#include "../utils.hpp"

using cubos::core::thread::Task;

TEST_CASE("thread::Task")
{
    SUBCASE("task which produces an int")
    {
        Task<int> task{};

        REQUIRE_FALSE(task.isDone());

        auto thread = std::thread{[&task]() {
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
            task.finish(42);
        }};

        SUBCASE("join before")
        {
            thread.join();
            REQUIRE(task.isDone());
            REQUIRE(task.result() == 42);
        }

        SUBCASE("join after")
        {
            task.isDone(); // May return either true or false.
            REQUIRE(task.result() == 42);
            thread.join();
        }
    }

    SUBCASE("check if task is destroyed properly")
    {
        Task<DetectDestructor> task{};
        bool destroyed = false;

        // A copy of the task is finished.
        {
            Task<DetectDestructor> task2{task};
            REQUIRE_FALSE(task.isDone());
            REQUIRE_FALSE(task2.isDone());
            task2.finish({&destroyed});
            REQUIRE_FALSE(destroyed);
            REQUIRE(task2.isDone());
        }
        REQUIRE(task.isDone());

        // Value isn't destroyed yet.
        REQUIRE_FALSE(destroyed);

        // Value is destroyed only after being accessed.
        task.result();
        REQUIRE(destroyed);
    }
}
