/// @file
/// @brief Class @ref cubos::core::memory::Opt.
/// @ingroup core-memory

#pragma once

#include <cubos/core/log.hpp>
#include <cubos/core/memory/move.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/nullable.hpp>
#include <cubos/core/reflection/type.hpp>

namespace cubos::core::memory
{
    /// @brief Wrapper for reference types to indicate that the given argument type is optional.
    /// @tparam T Argument type.
    /// @ingroup core-memory
    template <typename T>
    class Opt
    {
    public:
        CUBOS_REFLECT;

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

        /// @brief Move constructor.
        /// @param other Other optional.
        Opt(Opt<T>&& other) noexcept
            : mContains{other.mContains}
        {
            if (mContains)
            {
                new (&mValue) T(memory::move(other.mValue));
                other.reset();
            }
        }

        /// @brief Copy assignment.
        /// @param other Other optional.
        /// @return Reference to this.
        Opt<T>& operator=(const Opt<T>& other)
        {
            if (this != &other)
            {
                if (mContains)
                {
                    mValue.~T();
                }

                mContains = other.mContains;
                if (mContains)
                {
                    new (&mValue) T(other.mValue);
                }
            }

            return *this;
        }

        /// @brief Move assignment.
        /// @param other Other optional.
        /// @return Reference to this.
        Opt<T>& operator=(Opt<T>&& other) noexcept
        {
            if (this != &other)
            {
                if (mContains)
                {
                    mValue.~T();
                }

                mContains = other.mContains;
                if (mContains)
                {
                    new (&mValue) T(memory::move(other.mValue));
                    other.reset();
                }
            }

            return *this;
        }

        /// @brief Moves the given value into the optional, destroying any previously stored value.
        /// @param value Value.
        void replace(T value)
        {
            if (mContains)
            {
                mValue.~T();
            }

            mContains = true;
            new (&mValue) T(memory::move(value));
        }

        /// @brief Destroys the value, if any.
        void reset()
        {
            if (mContains)
            {
                mValue.~T();
                mContains = false;
            }
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

        /// @brief Gets the underlying value.
        ///
        /// Aborts if there's no value.
        ///
        /// @return Reference to value.
        const T& value() const
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

        /// @brief Compares with another optional value.
        /// @param other Other value.
        /// @return Whether they're equal.
        template <typename U = T> // See https://brevzin.github.io/c++/2021/11/21/conditional-members/
        requires(requires(const U& a, const U& b) { a == b; }) bool operator==(const Opt<U>& other) const
        {
            return (!mContains && !other.mContains) || (mContains && other.mContains && mValue == other.mValue);
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
} // namespace cubos::core::memory

CUBOS_REFLECT_TEMPLATE_IMPL((typename T), (cubos::core::memory::Opt<T>))
{
    using namespace cubos::core::reflection;
    using cubos::core::memory::Opt;

    return reflection::Type::create("cubos::core::memory::Opt<" + reflect<T>().name() + ">")
        .with(ConstructibleTrait::typed<Opt<T>>().withDefaultConstructor().withCopyConstructor().build())
        .with(NullableTrait{[](const void* obj) -> bool { return static_cast<const Opt<T>*>(obj)->contains(); },
                            [](void* obj) { static_cast<Opt<T>*>(obj)->reset(); }});
}
