#pragma once

#include <cubos/core/log.hpp>

namespace cubos::core::ecs
{
    /// System argument which provides read access to the given resource or component.
    /// Can be used as a pointer with both the `->` and `.` operators.
    template <typename T>
    class Read
    {
    public:
        /// Creates a new read argument.
        /// @param ref Reference to the resource or component.
        inline Read(const T& ref)
            : mRef(ref)
        {
        }

        /// @returns A pointer to the resource or component.
        inline const T* operator->() const
        {
            return &mRef;
        }

        /// @returns A reference to the resource or component.
        inline const T& operator*() const
        {
            return mRef;
        }

        /// @returns A reference to the resource or component.
        inline const T& get() const
        {
            return mRef;
        }

    private:
        const T& mRef; ///< Reference to the resource or component.
    };

    /// System argument which provides write access to the given resource or component.
    /// Can be used as a pointer with both the `->` and `.` operators.
    template <typename T>
    class Write
    {
    public:
        /// Creates a new write argument.
        /// @param ref Reference to the resource or component.
        inline Write(T& ref)
            : mRef(ref)
        {
        }

        /// @returns A pointer to the resource or component.
        inline T* operator->()
        {
            return &mRef;
        }

        /// @returns A reference to the resource or component.
        inline T& operator*()
        {
            return mRef;
        }

        /// @returns A reference to the resource or component.
        inline T& get()
        {
            return mRef;
        }

    private:
        T& mRef; ///< Reference to the resource or component.
    };

    /// System argument which provides optional read access to the given resource or component, if it exists.
    /// Can be used as a pointer with both the `->` and `.` operators.
    template <typename T>
    class OptRead
    {
    public:
        /// Creates a new optional read argument.
        /// @param ptr Pointer to the resource or component.
        inline OptRead(const T* ptr)
            : mPtr(ptr)
        {
        }

        /// @returns A reference to the resource or component.
        inline const T* operator->() const
        {
            return &this->get();
        }

        /// @returns A reference to the resource or component.
        inline const T& operator*() const
        {
            return this->get();
        }

        /// @returns A reference to the resource or component.
        inline const T& get() const
        {
            CUBOS_ASSERT(mPtr != nullptr, "Attempted to access a null optional resource or component");
            return *mPtr;
        }

        /// Checks if the resource or component exists.
        /// @returns True if the resource or component exists, false otherwise.
        inline operator bool() const
        {
            return mPtr != nullptr;
        }

    private:
        const T* mPtr; ///< Pointer to the resource or component.
    };

    /// System argument which provides optional write access to the given resource or component, if it exists.
    /// Can be used as a pointer with both the `->` and `.` operators.
    template <typename T>
    class OptWrite
    {
    public:
        /// Creates a new optional write argument.
        /// @param ptr Pointer to the resource or component.
        inline OptWrite(T* ptr)
            : mPtr(ptr)
        {
        }

        /// @returns A reference to the resource or component.
        inline T* operator->()
        {
            return &this->get();
        }

        /// @returns A reference to the resource or component.
        inline T& operator*()
        {
            return this->get();
        }

        /// @returns A reference to the resource or component.
        inline T& get()
        {
            CUBOS_ASSERT(mPtr != nullptr, "Attempted to access a null optional resource or component");
            return *mPtr;
        }

        /// Checks if the resource or component exists.
        /// @returns True if the resource or component exists, false otherwise.
        inline operator bool() const
        {
            return mPtr != nullptr;
        }

    private:
        T* mPtr; ///< Pointer to the resource or component.
    };
} // namespace cubos::core::ecs
