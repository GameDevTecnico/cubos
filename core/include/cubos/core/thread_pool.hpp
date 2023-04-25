#ifndef CUBOS_CORE_THREAD_POOL_HPP
#define CUBOS_CORE_THREAD_POOL_HPP

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
        std::vector<std::thread> threads;        ///< Threads in the pool.
        std::deque<std::function<void()>> tasks; ///< Queue of tasks to execute.

        std::mutex mutex;                  ///< Protects the tasks vector.
        std::condition_variable new_task;  ///< Notifies threads when new tasks may be available.
        std::condition_variable task_done; ///< Notifies threads when a task has finished executing.

        std::atomic<std::size_t> numTasks; ///< Number of tasks currently being executed.
        bool stop;                    ///< Set to true when the thread pool is being destroyed.
    };
} // namespace cubos::core

#endif // CUBOS_CORE_THREAD_POOL_HPP
