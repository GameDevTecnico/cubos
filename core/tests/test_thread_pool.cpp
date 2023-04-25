#include <gtest/gtest.h>
#include <cubos/core/thread_pool.hpp>

using namespace cubos::core;

const size_t NumThreads = 4;
const size_t NumTasks = 32;

TEST(Cubos_Thread_Pool, Thead_Pool_Wait_And_Destroy)
{
    std::atomic<size_t> done = 0;

    {
        auto pool = ThreadPool(NumThreads);
        for (size_t i = 0; i < NumTasks; i++)
        {
            pool.addTask([&]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                done += 1;
            });
        }

        pool.wait();
        EXPECT_EQ(done, NumTasks);

        done = 0;
        for (size_t i = 0; i < NumTasks; i++)
        {
            pool.addTask([&]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                done += 1;
            });
        }
    }

    EXPECT_EQ(done, NumTasks);
}
