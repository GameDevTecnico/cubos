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
        inline ReadGuard(const T& object, Lock&& lock) : object(object), lock(std::move(lock))
        {
        }

        /// @param other The other guard to move from.
        inline ReadGuard(ReadGuard&& other) : object(other.object), lock(std::move(other.lock))
        {
        }

        /// @returns The underlying object.
        inline const T& get() const
        {
            return this->object;
        }

        /// @returns The underlying object.
        inline const T& operator*() const
        {
            return this->object;
        }

        /// @returns The underlying object.
        inline const T* operator->() const
        {
            return &this->object;
        }

    private:
        const T& object; ///< Object being guarded.
        Lock lock;       ///< Lock used to guard the object.
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
        inline WriteGuard(T& object, Lock&& lock) : object(object), lock(std::move(lock))
        {
        }

        /// @param other The other guard to move from.
        inline WriteGuard(WriteGuard&& other) : object(other.object), lock(std::move(other.lock))
        {
        }

        /// @returns The underlying object.
        inline T& get()
        {
            return this->object;
        }

        /// @returns The underlying object.
        inline T& operator*()
        {
            return this->object;
        }

        /// @returns The underlying object.
        inline T* operator->()
        {
            return &this->object;
        }

        /// @returns The underlying object.
        inline const T& get() const
        {
            return this->object;
        }

        /// @returns The underlying object.
        inline const T& operator*() const
        {
            return this->object;
        }

        /// @returns The underlying object.
        inline const T* operator->() const
        {
            return &this->object;
        }

    private:
        T& object; ///< Object being guarded.
        Lock lock; ///< Lock used to guard the object.
    };
} // namespace cubos::core::memory
