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
            : ref(ref)
        {
        }

        /// @returns A pointer to the resource or component.
        inline const T* operator->() const
        {
            return &this->ref;
        }

        /// @returns A reference to the resource or component.
        inline const T& operator*() const
        {
            return this->ref;
        }

        /// @returns A reference to the resource or component.
        inline const T& get() const
        {
            return this->ref;
        }

    private:
        const T& ref; ///< Reference to the resource or component.
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
            : ref(ref)
        {
        }

        /// @returns A pointer to the resource or component.
        inline T* operator->()
        {
            return &this->ref;
        }

        /// @returns A reference to the resource or component.
        inline T& operator*()
        {
            return this->ref;
        }

        /// @returns A reference to the resource or component.
        inline T& get()
        {
            return this->ref;
        }

    private:
        T& ref; ///< Reference to the resource or component.
    };

    /// System argument which provides optional read access to the given resource or component, if it exists.
    /// Can be used as a pointer with both the `->` and `.` operators.
    template <typename T>
    class OptRead
    {
    public:
        /// Creates a new optional read argument.
        /// @param ref Reference to the resource or component.
        inline OptRead(const T* ref)
            : ref(ref)
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
            CUBOS_ASSERT(this->ref != nullptr, "Attempted to access a null optional resource or component");
            return *this->ref;
        }

        /// Checks if the resource or component exists.
        /// @returns True if the resource or component exists, false otherwise.
        inline operator bool() const
        {
            return this->ref != nullptr;
        }

    private:
        const T* ref; ///< Reference to the resource or component.
    };

    /// System argument which provides optional write access to the given resource or component, if it exists.
    /// Can be used as a pointer with both the `->` and `.` operators.
    template <typename T>
    class OptWrite
    {
    public:
        /// Creates a new optional write argument.
        /// @param ref Reference to the resource or component.
        inline OptWrite(T* ref)
            : ref(ref)
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
            CUBOS_ASSERT(this->ref != nullptr, "Attempted to access a null optional resource or component");
            return *this->ref;
        }

        /// Checks if the resource or component exists.
        /// @returns True if the resource or component exists, false otherwise.
        inline operator bool() const
        {
            return this->ref != nullptr;
        }

    private:
        T* ref; ///< Reference to the resource or component.
    };
} // namespace cubos::core::ecs
