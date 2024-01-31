/// @file
/// @brief Class @ref cubos::core::ecs::Opt.
/// @ingroup core-ecs-query

#pragma once

#include <cubos/core/log.hpp>
#include <cubos/core/memory/move.hpp>

namespace cubos::core::ecs
{
    /// @brief Wrapper for reference types to indicate that the given argument type is optional in a query.
    /// @tparam T Argument type.
    /// @ingroup core-ecs-query
    template <typename T>
    class Opt
    {
    public:
        ~Opt()
        {
            if (mContains)
            {
                mValue.~T();
            }
        }

        /// @brief Constructs without a value.
        Opt()
            : mContains{false}
        {
        }

        /// @brief Constructs with a value.
        /// @param value Value.
        Opt(T value)
            : mContains{true}
            , mValue{memory::move(value)}
        {
        }

        /// @brief Copy constructor.
        /// @param other Other optional.
        Opt(const Opt<T>& other)
            : mContains{other.mContains}
        {
            if (mContains)
            {
                new (&mValue) T(other.mValue);
            }
        }

        /// @brief Moves the given value into the optional, destroying any previously stored value.
        void replace(T value)
        {
            if (mContains)
            {
                mValue.~T();
            }

            new (&mValue) T(memory::move(value));
        }

        /// @brief Checks if the optional contains a value.
        /// @return Whether the optional contains a value.
        bool contains() const
        {
            return mContains;
        }

        /// @brief Gets the underlying value.
        ///
        /// Aborts if there's no value.
        ///
        /// @return Reference to value.
        T& value()
        {
            CUBOS_ASSERT(mContains, "Optional does not contain value");
            return mValue;
        }

        /// @copydoc contains()
        operator bool() const
        {
            return this->contains();
        }

        /// @copydoc value()
        T& operator*()
        {
            return this->value();
        }

        /// @copydoc value()
        T* operator->()
        {
            return &this->value();
        }

    private:
        bool mContains;

        // Inside a union to prevent T's destructor from being called when it wasn't initialized.
        union {
            T mValue;
        };
    };

    template <typename T>
    class Opt<T&>
    {
    public:
        Opt()
            : mValue{nullptr}
        {
        }

        Opt(T& value)
            : mValue{&value}
        {
        }

        Opt(const Opt<T&>& other)
            : mValue{other.mValue}
        {
        }

        bool contains() const
        {
            return mValue != nullptr;
        }

        T& value()
        {
            CUBOS_ASSERT(mValue != nullptr, "Optional does not contain value");
            return *mValue;
        }

        operator bool() const
        {
            return this->contains();
        }

        T& operator*()
        {
            return this->value();
        }

        T* operator->()
        {
            return &this->value();
        }

    private:
        T* mValue;
    };
} // namespace cubos::core::ecs
