/// @file
/// @brief Class @ref cubos::core::ThreadPool.
/// @ingroup core

#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace cubos::core
{
    /// @brief Manages a pool of threads, to which tasks can be submitted.
    /// @note Blocks on tasks to finish on destruction.
    /// @ingroup core
    class ThreadPool final
    {
    public:
        ~ThreadPool();

        /// @brief Constructs a pool with @p numThreads, starting them immediately.
        /// @param numThreads Number of threads to create.
        ThreadPool(std::size_t numThreads);

        /// @brief Forbid copy construction.
        ThreadPool(const ThreadPool&) = delete;

        /// @brief Forbid move construction.
        ThreadPool(ThreadPool&&) = delete;

        /// @brief Adds a task to the thread pool. Starts when a thread becomes available.
        /// @param task Task to add.
        void addTask(std::function<void()> task);

        /// @brief Blocks until all tasks finish.
        void wait();

    private:
        std::vector<std::thread> mThreads;        ///< Threads in the pool.
        std::deque<std::function<void()>> mTasks; ///< Queue of tasks to execute.

        std::mutex mMutex;                 ///< Protects the tasks vector.
        std::condition_variable mNewTask;  ///< Notifies threads when new tasks may be available.
        std::condition_variable mTaskDone; ///< Notifies threads when a task has finished executing.

        std::atomic<std::size_t> mNumTasks; ///< Number of tasks currently being executed.
        bool mStop;                         ///< Set to true when the thread pool is being destroyed.
    };
} // namespace cubos::core
