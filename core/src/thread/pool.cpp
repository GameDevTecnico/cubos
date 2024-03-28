#include <cubos/core/thread/pool.hpp>

using cubos::core::thread::ThreadPool;

ThreadPool::ThreadPool(std::size_t numThreads)
{
    mThreads.reserve(numThreads);
    mNumTasks = 0;
    mStop = false;

    for (std::size_t i = 0; i < numThreads; i++)
    {
        mThreads.emplace_back([this]() {
            while (true)
            {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(mMutex);

                    // Wait until a task is available, or the thread pool is being destroyed.
                    mNewTask.wait(lock, [this]() { return mStop || !mTasks.empty(); });
                    if (mStop && mTasks.empty())
                    {
                        return; // Thread pool is being destroyed, and there are no more tasks to execute.
                    }
                    task = std::move(mTasks.front());
                    mTasks.pop_front();
                    mNumTasks += 1; // A task is being executed.
                }

                task();
                mNumTasks -= 1; // Task has finished executing.

                // Signal that a thread has finished executing a task.
                mTaskDone.notify_one();
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    mStop = true;
    mNewTask.notify_all();
    for (auto& thread : mThreads)
    {
        thread.join();
    }
}

void ThreadPool::addTask(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mTasks.push_back(std::move(task));
    }
    mNewTask.notify_one();
}

void ThreadPool::wait()
{
    std::unique_lock<std::mutex> lock(mMutex);
    mTaskDone.wait(lock, [this]() { return mNumTasks == 0 && mTasks.empty(); });
}
