#include <gtest/gtest.h>
#include <cubos/core/thread_pool.hpp>

using namespace cubos::core;

const size_t numThreads = 4;
const size_t numTasks = 32;

TEST(Cubos_Thread_Pool, Thead_Pool_Wait_And_Destroy)
{
    std::atomic<size_t> done = 0;

    {
        auto pool = ThreadPool(numThreads);
        for (size_t i = 0; i < numTasks; i++)
        {
            pool.addTask([&]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                done += 1;
            });
        }

        pool.wait();
        EXPECT_EQ(done, numTasks);

        done = 0;
        for (size_t i = 0; i < numTasks; i++)
        {
            pool.addTask([&]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                done += 1;
            });
        }
    }

    EXPECT_EQ(done, numTasks);
}
