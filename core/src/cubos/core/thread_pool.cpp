#include <cubos/core/thread_pool.hpp>

using namespace cubos::core;

ThreadPool::ThreadPool(size_t numThreads)
{
    this->threads.reserve(numThreads);
    this->numTasks = 0;
    this->stop = false;

    for (size_t i = 0; i < numThreads; i++)
    {
        this->threads.emplace_back([this]() {
            while (true)
            {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->mutex);

                    // Wait until a task is available, or the thread pool is being destroyed.
                    this->new_task.wait(lock, [this]() { return this->stop || !this->tasks.empty(); });
                    if (this->stop && this->tasks.empty())
                    {
                        return; // Thread pool is being destroyed, and there are no more tasks to execute.
                    }
                    task = std::move(this->tasks.front());
                    this->tasks.pop_front();
                    this->numTasks += 1; // A task is being executed.
                }

                task();
                this->numTasks -= 1; // Task has finished executing.

                // Signal that a thread has finished executing a task.
                this->task_done.notify_one();
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    this->stop = true;
    this->new_task.notify_all();
    for (auto& thread : this->threads)
    {
        thread.join();
    }
}

void ThreadPool::addTask(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        this->tasks.push_back(std::move(task));
    }
    this->new_task.notify_one();
}

void ThreadPool::wait()
{
    std::unique_lock<std::mutex> lock(this->mutex);
    this->task_done.wait(lock, [this]() { return this->numTasks == 0 && this->tasks.empty(); });
}
