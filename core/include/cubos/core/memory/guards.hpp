#pragma once

namespace cubos::core::memory
{
    /// Utility class to provide safe read-only access to an object using a lock.
    ///
    /// @details This class was created because there are multiple parts of the code that need to provide
    /// access to objects in a thread-safe manner.
    ///
    /// Usage example:
    ///     using AssetMetaRead = core::memory::ReadGuard<AssetMeta, std::shared_lock<std::shared_mutex>>;
    ///
    /// @tparam T The type of the object to guard.
    /// @tparam Lock The type of the lock to use.
    template <typename T, typename Lock>
    class ReadGuard
    {
    public:
        /// @param object The object to guard.
        /// @param lock The lock to use.
        inline ReadGuard(const T& object, Lock&& lock)
            : mObject(object)
            , mLock(std::move(lock))
        {
        }

        /// @param other The other guard to move from.
        inline ReadGuard(ReadGuard&& other) noexcept
            : mObject(other.object)
            , mLock(std::move(other.lock))
        {
        }

        /// @returns The underlying object.
        inline const T& get() const
        {
            return mObject;
        }

        /// @returns The underlying object.
        inline const T& operator*() const
        {
            return mObject;
        }

        /// @returns The underlying object.
        inline const T* operator->() const
        {
            return &mObject;
        }

    private:
        const T& mObject; ///< Object being guarded.
        Lock mLock;       ///< Lock used to guard the object.
    };

    /// Utility class to provide safe read-write access to an object using a lock.
    ///
    /// @details This class was created because there are multiple parts of the code that need to provide
    /// access to objects in a thread-safe manner.
    ///
    /// Usage example:
    ///     using AssetMetaWrite = core::memory::WriteGuard<AssetMeta, std::unique_lock<std::shared_mutex>>;
    ///
    /// @tparam T The type of the object to guard.
    /// @tparam Lock The type of the lock to use.
    template <typename T, typename Lock>
    class WriteGuard
    {
    public:
        /// @param object The object to guard.
        /// @param lock The lock to use.
        inline WriteGuard(T& object, Lock&& lock)
            : mObject(object)
            , mLock(std::move(lock))
        {
        }

        /// @param other The other guard to move from.
        inline WriteGuard(WriteGuard&& other) noexcept
            : mObject(other.object)
            , mLock(std::move(other.lock))
        {
        }

        /// @returns The underlying object.
        inline T& get()
        {
            return mObject;
        }

        /// @returns The underlying object.
        inline T& operator*()
        {
            return mObject;
        }

        /// @returns The underlying object.
        inline T* operator->()
        {
            return &mObject;
        }

        /// @returns The underlying object.
        inline const T& get() const
        {
            return mObject;
        }

        /// @returns The underlying object.
        inline const T& operator*() const
        {
            return mObject;
        }

        /// @returns The underlying object.
        inline const T* operator->() const
        {
            return &mObject;
        }

    private:
        T& mObject; ///< Object being guarded.
        Lock mLock; ///< Lock used to guard the object.
    };
} // namespace cubos::core::memory
