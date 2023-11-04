/// @file
/// @brief Class @ref cubos::core::memory::Function.
/// @ingroup core-memory

#pragma once

#include <cstddef>

#include <cubos/core/log.hpp>
#include <cubos/core/memory/move.hpp>

namespace cubos::core::memory
{
    template <typename>
    class Function final
    {
    };

    /// @brief Generic function pointer which can also store capturing lambda functions.
    ///
    /// Should be used with care as this class needs to do heap allocations to store captured data.
    ///
    /// @note This is a replacement for `std::function`, which allows us to avoid including the
    /// entire `<functional>` header.
    ///
    /// @tparam R Return type.
    /// @tparam Ts Argument types.
    /// @ingroup core-memory
    template <typename R, typename... Ts>
    class Function<R(Ts...)> final
    {
    public:
        ~Function()
        {
            if (mFunction != nullptr)
            {
                mFree(mFunction);
            }
        }

        /// @brief Default constructs.
        Function() noexcept = default;

        /// @brief Move constructs.
        /// @param other Other.
        Function(Function&& other) noexcept
            : mFunction{other.mFunction}
            , mInvoke{other.mInvoke}
            , mFree{other.mFree}
        {
            other.mFunction = nullptr;
        }

        /// @brief Constructs.
        /// @tparam F Function type.
        /// @param function Function.
        template <typename F>
        Function(F function)
        {
            mFunction = new F(memory::move(function));
            mInvoke = [](void* function, Ts... args) -> R {
                return (*static_cast<F*>(function))(memory::forward<Ts>(args)...);
            };
            mFree = [](void* function) { delete static_cast<F*>(function); };
        }

        /// @brief Frees the underlying function.
        /// @return This.
        Function& operator=(std::nullptr_t /*unused*/)
        {
            if (mFunction != nullptr)
            {
                mFree(mFunction);
                mFunction = nullptr;
            }

            return *this;
        }

        /// @brief Move assigns.
        /// @param other Other.
        /// @return This.
        Function& operator=(Function&& other) noexcept
        {
            if (mFunction != nullptr)
            {
                mFree(mFunction); // NOLINT(clang-analyzer-core.CallAndMessage)
            }

            mFunction = other.mFunction;
            mInvoke = other.mInvoke;
            mFree = other.mFree;
            other.mFunction = nullptr;
            return *this;
        }

        /// @brief Invokes the underlying function.
        ///
        /// Must not be null.
        ///
        /// @param args Argument values.
        /// @return Return value.
        R operator()(Ts... args)
        {
            CUBOS_ASSERT(mFunction != nullptr, "Function must not be null");
            return mInvoke(mFunction, memory::forward<Ts>(args)...);
        }

        /// @brief Checks if the function is null.
        /// @return Whether the function is null.
        bool operator==(std::nullptr_t /*unused*/) const
        {
            return mFunction == nullptr;
        }

    private:
        void* mFunction{nullptr};
        R (*mInvoke)(void*, Ts...);
        void (*mFree)(void*);
    };
} // namespace cubos::core::memory
