/// @file
/// @brief Classes @ref cubos::core::memory::ReadGuard and @ref cubos::core::memory::WriteGuard.
/// @ingroup core-memory

#pragma once

namespace cubos::core::memory
{
    /// @brief Provides safe read-only access to an object using a lock.
    /// @tparam T Guarded object type.
    /// @tparam Lock Held lock type.
    ///
    /// This class was created because there are multiple parts of the code that need to provide
    /// access to objects in a thread-safe manner.
    ///
    /// ## Usage example
    ///
    /// @code{.cpp}
    ///     using AssetMetaRead = core::memory::ReadGuard<AssetMeta, std::shared_lock<std::shared_mutex>>;
    /// @endcode
    ///
    /// @ingroup core-memory
    template <typename T, typename Lock>
    class ReadGuard
    {
    public:
        /// @brief Constructor.
        /// @param object Object to guard.
        /// @param lock Lock to hold.
        inline ReadGuard(const T& object, Lock&& lock)
            : mObject(object)
            , mLock(std::move(lock))
        {
        }

        /// @brief Move constructor.
        /// @param other Guard to move from.
        inline ReadGuard(ReadGuard&& other) noexcept
            : mObject(other.mObject)
            , mLock(std::move(other.mLock))
        {
        }

        /// @brief Gets a reference to the underlying object.
        /// @return Underlying object.
        inline const T& get() const
        {
            return mObject;
        }

        /// @brief Gets a reference to the underlying object.
        /// @return Underlying object.
        inline const T& operator*() const
        {
            return mObject;
        }

        /// @brief Gets a pointer to the underlying object.
        /// @return Underlying object.
        inline const T* operator->() const
        {
            return &mObject;
        }

    private:
        const T& mObject; ///< Object being guarded.
        Lock mLock;       ///< Lock used to guard the object.
    };

    /// @brief Provides safe read-write access to an object using a lock.
    /// @tparam T Guarded object type.
    /// @tparam Lock Held lock type.
    ///
    /// This class was created because there are multiple parts of the code that need to provide
    /// access to objects in a thread-safe manner.
    ///
    /// ## Usage example
    ///
    /// @code{.cpp}
    ///     using AssetMetaWrite = core::memory::WriteGuard<AssetMeta, std::unique_lock<std::shared_mutex>>;
    /// @endcode
    ///
    /// @ingroup core-memory
    template <typename T, typename Lock>
    class WriteGuard
    {
    public:
        /// @brief Constructor.
        /// @param object Object to guard.
        /// @param lock Lock to hold.
        inline WriteGuard(T& object, Lock&& lock)
            : mObject(object)
            , mLock(std::move(lock))
        {
        }

        /// @brief Move constructs.
        /// @param other Guard to move from.
        inline WriteGuard(WriteGuard&& other) noexcept
            : mObject(other.object)
            , mLock(std::move(other.lock))
        {
        }

        /// @brief Gets a reference to the underlying object.
        /// @return Underlying object.
        inline T& get()
        {
            return mObject;
        }

        /// @brief Gets a reference to the underlying object.
        /// @return Underlying object.
        inline T& operator*()
        {
            return mObject;
        }

        /// @brief Gets a pointer to the underlying object.
        /// @return Underlying object.
        inline T* operator->()
        {
            return &mObject;
        }

        /// @brief Gets a reference to the underlying object.
        /// @return Underlying object.
        inline const T& get() const
        {
            return mObject;
        }

        /// @brief Gets a reference to the underlying object.
        /// @return Underlying object.
        inline const T& operator*() const
        {
            return mObject;
        }

        /// @brief Gets a pointer to the underlying object.
        /// @return Underlying object.
        inline const T* operator->() const
        {
            return &mObject;
        }

    private:
        T& mObject; ///< Object being guarded.
        Lock mLock; ///< Lock used to guard the object.
    };
} // namespace cubos::core::memory
