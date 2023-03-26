#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace cubos::core
{
    /// Manages a pool of threads, to which tasks can be submitted.
    /// The pool waits for all tasks to finish before being destroyed.
    class ThreadPool final
    {
    public:
        /// @param numThreads Number of threads to create.
        ThreadPool(std::size_t numThreads);
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool(ThreadPool&&) = delete;
        ~ThreadPool();

        /// Adds a task to the thread pool. The task starts executing as soon as a thread is available.
        void addTask(std::function<void()> task);

        /// Waits for all tasks to finish.
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
