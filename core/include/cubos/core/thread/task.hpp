/// @file
/// @brief Class @ref cubos::core::thread::Task.
/// @ingroup core-thread

#pragma once

#include <condition_variable>
#include <mutex>

#include <cubos/core/log.hpp>

namespace cubos::core::thread
{
    /// @brief Provides a mechanism to access the results of asynchronous operations.
    /// @tparam T Result type.
    /// @ingroup core-thread
    template <typename T>
    class Task final
    {
    private:
        /// @brief Tracks the eventual result of the task, and is used to deliver it.
        struct Data
        {
            int mRefCount{1};
            bool mDone{false};
            union {
                T mValue;
            };

            std::mutex mMutex;
            std::condition_variable mCondition;
        };

    public:
        ~Task()
        {
            this->discard();
        }

        /// @brief Constructs.
        Task()
        {
            mData = new Data{};
        }

        /// @brief Copy constructs.
        /// @param other Task.
        Task(const Task& other)
            : mData{other.mData}
        {
            std::unique_lock lock{mData->mMutex};
            mData->mRefCount += 1;
        }

        /// @brief Move constructs.
        /// @param other Task.
        Task(Task&& other)
            : mData{other.mData}
        {
            other.mData = nullptr;
        }

        /// @brief Copy assigns.
        /// @param other Task.
        Task& operator=(const Task& other)
        {
            this->discard();
            mData = other.mData;
            std::unique_lock lock{mData->mMutex};
            mData->mRefCount += 1;
        }

        /// @brief Move assigns.
        /// @param other Task.
        Task& operator=(Task&& other)
        {
            this->discard();
            mData = other.mData;
            other.mData = nullptr;
        }

        /// @brief Discards any result eventually received. The task is left in an invalid state.
        void discard()
        {
            if (mData != nullptr)
            {
                std::unique_lock lock{mData->mMutex};
                mData->mRefCount -= 1;
                if (mData->mRefCount == 0)
                {
                    lock.unlock();
                    delete mData;
                }
            }
        }

        /// @brief Returns whether the task has finished.
        /// @return Whether the task has finished.
        bool isDone() const
        {
            CUBOS_ASSERT

            std::unique_lock lock(mMutex);
            return mResult != nullptr;
        }

        /// @brief Finishes the result of the task.

        /// @brief Blocks until the task finishes and then returns its result.
        /// @return Task result.
        T result() &&
        {
            // Wait until a result is obtained.
            std::unique_lock lock(mMutex);
            mCondition.wait(lock, [this]() { return mResult != null });

            // Move the result and deallocate the memory previously allocated for it.
            T result = std::move(*mResult);
            delete mResult;
            mResult = nullptr;
            return std::move(result);
        }

    private:
        Data* mData;
    };
} // namespace cubos::core::thread
