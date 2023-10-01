/// @file
/// @brief Class @ref cubos::core::ecs::Read and related types.
/// @ingroup core-ecs-system

#pragma once

#include <cubos/core/log.hpp>

namespace cubos::core::ecs
{
    /// @brief System argument which provides read access to the resource @p T, or query argument
    /// which provides read access to the component @p T.
    ///
    /// Can be used as a pointer with both the `->` and `*` operators.
    ///
    /// @tparam T Resource or component type.
    /// @ingroup core-ecs-system
    template <typename T>
    class Read
    {
    public:
        /// @brief Creates a new read argument.
        /// @param ref Reference to the resource or component.
        inline Read(const T& ref)
            : mRef(ref)
        {
        }

        /// @brief Accesses the resource or component.
        /// @return Pointer to the resource or component.
        inline const T* operator->() const
        {
            return &mRef;
        }

        /// @brief Accesses the resource or component.
        /// @return Reference to the resource or component.
        inline const T& operator*() const
        {
            return mRef;
        }

    private:
        const T& mRef; ///< Reference to the resource or component.
    };

    /// @brief System argument which provides write access to the resource @p T, or query argument
    /// which provides write access to the component @p T.
    ///
    /// Can be used as a pointer with both the `->` and `*` operators.
    ///
    /// @tparam T Resource or component type.
    /// @ingroup core-ecs-system
    template <typename T>
    class Write
    {
    public:
        /// @brief Creates a new write argument.
        /// @param ref Reference to the resource or component.
        inline Write(T& ref)
            : mRef(ref)
        {
        }

        /// @brief Accesses the resource or component.
        /// @return Pointer to the resource or component.
        inline T* operator->()
        {
            return &mRef;
        }

        /// @brief Accesses the resource or component.
        /// @return Reference to the resource or component.
        inline T& operator*()
        {
            return mRef;
        }

    private:
        T& mRef; ///< Reference to the resource or component.
    };

    /// @brief System argument which provides read access to the resource @p T if it exists, or
    /// query argument which provides read access to the component @p T if it exists.
    ///
    /// While the @ref Read demands that the resource or component exists, this argument does not.
    /// Can be used as a pointer with both the `->` and `*` operators.
    ///
    /// @tparam T Resource or component type.
    /// @ingroup core-ecs-system
    template <typename T>
    class OptRead
    {
    public:
        /// @brief Creates a new optional read argument. @p ptr should be null if the resource or
        /// component does not exist.
        /// @param ptr Pointer to the resource or component.
        inline OptRead(const T* ptr)
            : mPtr(ptr)
        {
        }

        /// @brief Accesses the resource or component, aborting if it does not exist.
        /// @return Reference to the resource or component.
        inline const T* operator->() const
        {
            return &this->get();
        }

        /// @return Reference to the resource or component, aborting if it does not exist.
        inline const T& operator*() const
        {
            return this->get();
        }

        /// @brief Checks if the resource or component exists.
        /// @return Whether the resource or component exists.
        inline operator bool() const
        {
            return mPtr != nullptr;
        }

    private:
        const T* mPtr; ///< Pointer to the resource or component.

        /// @brief Accesses the resource or component, aborting if it does not exist.
        /// @return Reference to the resource or component.
        inline const T& get() const
        {
            CUBOS_ASSERT(mPtr != nullptr, "Attempted to access a null optional resource or component");
            return *mPtr;
        }
    };

    /// @brief System argument which provides write access to the resource @p T if it exists, or
    /// query argument which provides write access to the component @p T if it exists.
    ///
    /// While the @ref Write demands that the resource or component exists, this argument does not.
    /// Can be used as a pointer with both the `->` and `*` operators.
    ///
    /// @tparam T Resource or component type.
    /// @ingroup core-ecs-system
    template <typename T>
    class OptWrite
    {
    public:
        /// @brief Creates a new optional write argument. @p ptr should be null if the resource or
        /// component does not exist.
        /// @param ptr Pointer to the resource or component.
        inline OptWrite(T* ptr)
            : mPtr(ptr)
        {
        }

        /// @brief Accesses the resource or component, aborting if it does not exist.
        /// @return Reference to the resource or component.
        inline T* operator->()
        {
            return &this->get();
        }

        /// @brief Accesses the resource or component, aborting if it does not exist.
        /// @return Reference to the resource or component.
        inline T& operator*()
        {
            return this->get();
        }

        /// @brief Checks if the resource or component exists.
        /// @return Whether the resource or component exists.
        inline operator bool() const
        {
            return mPtr != nullptr;
        }

    private:
        T* mPtr; ///< Pointer to the resource or component.

        /// @brief Accesses the resource or component, aborting if it does not exist.
        /// @return Reference to the resource or component.
        inline T& get()
        {
            CUBOS_ASSERT(mPtr != nullptr, "Attempted to access a null optional resource or component");
            return *mPtr;
        }
    };
} // namespace cubos::core::ecs
