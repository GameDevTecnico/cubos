/// @file
/// @brief Class @ref cubos::core::thread::Task.
/// @ingroup core-thread

#pragma once

#include <condition_variable>
#include <mutex>

#include <cubos/core/tel/logging.hpp>

namespace cubos::core::thread
{
    /// @brief Provides a mechanism to access the results of asynchronous operations.
    /// @tparam T Result type.
    /// @ingroup core-thread
    template <typename T>
    class Task final
    {
    public:
        ~Task()
        {
            this->discard();
        }

        /// @brief Constructs.
        Task()
        {
            mData = new Data();
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
        Task(Task&& other) noexcept
            : mData{other.mData}
        {
            other.mData = nullptr;
        }

        /// @brief Copy assigns.
        /// @param other Task.
        Task& operator=(const Task& other)
        {
            if (this == &other)
            {
                return *this;
            }

            this->discard();
            mData = other.mData;
            std::unique_lock lock{mData->mMutex};
            mData->mRefCount += 1;
            return *this;
        }

        /// @brief Move assigns.
        /// @param other Task.
        Task& operator=(Task&& other) noexcept
        {
            if (this == &other)
            {
                return *this;
            }

            this->discard();
            mData = other.mData;
            other.mData = nullptr;
            return *this;
        }

        /// @brief Finishes the task, setting its result and notifying a waiting thread.
        /// @param value Task result.
        void finish(T value)
        {
            CUBOS_ASSERT(mData != nullptr, "Task has been discarded");

            // Set the result and notify one waiting thread.
            std::unique_lock lock(mData->mMutex);
            CUBOS_ASSERT(!mData->mDone, "Task has already been finished");

            new (&mData->mValue) T(std::move(value));
            mData->mDone = true;
            mData->mCondition.notify_all();
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
                mData = nullptr;
            }
        }

        /// @brief Returns whether the task has finished.
        /// @return Whether the task has finished.
        bool isDone() const
        {
            CUBOS_ASSERT(mData != nullptr, "Task has been discarded");
            std::unique_lock lock(mData->mMutex);
            return mData->mDone;
        }

        /// @brief Blocks until the task finishes and then returns its result.
        /// @return Task result.
        T result()
        {
            CUBOS_ASSERT(mData != nullptr, "Task has been discarded");

            // Wait until a result is obtained. When it is, consume it.
            {
                std::unique_lock lock(mData->mMutex);
                mData->mCondition.wait(lock, [this]() { return mData->mDone; });
                CUBOS_ASSERT(!mData->mConsumed, "Task result has already been consumed");
                mData->mConsumed = true;
            }

            // Move the result and reset the task.
            T result = std::move(mData->mValue);
            this->discard();
            return result;
        }

    private:
        /// @brief Tracks the eventual result of the task, and is used to deliver it.
        struct Data
        {
            int mRefCount{1};
            bool mDone{false};
            bool mConsumed{false};
            union {
                T mValue;
            };

            std::mutex mMutex{};
            std::condition_variable mCondition{};

            // NOLINTBEGIN(modernize-use-equals-default)
            Data()
            {
            }
            // NOLINTEND(modernize-use-equals-default)

            ~Data()
            {
                if (mDone)
                {
                    mValue.~T();
                }
            }
        };

        Data* mData;
    };
} // namespace cubos::core::thread
