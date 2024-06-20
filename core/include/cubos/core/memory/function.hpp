/// @file
/// @brief Class @ref cubos::core::memory::Function.
/// @ingroup core-memory

#pragma once

#include <cstddef>

#include <cubos/core/memory/move.hpp>
#include <cubos/core/tel/logging.hpp>

namespace cubos::core::memory
{
    /// @brief Generic function pointer which can also store capturing lambda functions.
    ///
    /// Should be used with care as this class needs to do heap allocations to store captured data.
    ///
    /// @note This is a replacement for `std::function`, which allows us to avoid including the
    /// entire `<functional>` header.
    ///
    /// @ingroup core-memory
    template <typename>
    class Function final
    {
    };

    template <typename I>
    class FunctionBase
    {
    public:
        ~FunctionBase()
        {
            if (mFunction != nullptr)
            {
                mFree(mFunction);
            }
        }

        /// @brief Default constructs.
        FunctionBase() noexcept = default;

        /// @brief Null constructor.
        FunctionBase(std::nullptr_t) noexcept
        {
        }

        /// @brief Move constructs.
        /// @param other Other.
        FunctionBase(FunctionBase&& other) noexcept
            : mFunction{other.mFunction}
            , mInvoke{other.mInvoke}
            , mFree{other.mFree}
            , mCopy{other.mCopy}
        {
            other.mFunction = nullptr;
        }

        /// @brief Copy constructs.
        /// @param other Other.
        FunctionBase(const FunctionBase& other)
            : mInvoke{other.mInvoke}
            , mFree{other.mFree}
            , mCopy{other.mCopy}
        {
            if (other.mFunction != nullptr)
            {
                CUBOS_ASSERT(other.mCopy != nullptr, "Function must be copyable");
                mFunction = other.mCopy(other.mFunction);
            }
        }

        /// @brief Constructs.
        /// @param function Function.
        /// @param invoke Invoke.
        /// @param free Free.
        /// @param copy Copy.
        FunctionBase(void* function, I invoke, void (*free)(void*), void* (*copy)(const void*))
            : mFunction{function}
            , mInvoke{invoke}
            , mFree{free}
            , mCopy{copy}
        {
        }

        /// @brief Frees the underlying function.
        /// @return This.
        FunctionBase& operator=(std::nullptr_t /*unused*/)
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
        FunctionBase& operator=(FunctionBase&& other) noexcept
        {
            if (this == &other)
            {
                return *this;
            }

            if (mFunction != nullptr)
            {
                mFree(mFunction); // NOLINT(clang-analyzer-core.CallAndMessage)
            }

            mFunction = other.mFunction;
            mInvoke = other.mInvoke;
            mFree = other.mFree;
            mCopy = other.mCopy;
            other.mFunction = nullptr;
            return *this;
        }

        /// @brief Copy assigns.
        /// @param other Other.
        /// @return This.
        FunctionBase& operator=(const FunctionBase& other)
        {
            if (this == &other)
            {
                return *this;
            }

            if (mFunction != nullptr)
            {
                mFree(mFunction); // NOLINT(clang-analyzer-core.CallAndMessage)
                mFunction = nullptr;
            }

            if (other.mFunction != nullptr)
            {
                CUBOS_ASSERT(other.mCopy != nullptr, "Function must be copyable");
                mFunction = other.mCopy(other.mFunction);
                mInvoke = other.mInvoke;
                mFree = other.mFree;
                mCopy = other.mCopy;
            }

            return *this;
        }

        /// @brief Checks if the function is null.
        /// @return Whether the function is null.
        bool operator==(std::nullptr_t /*unused*/) const
        {
            return mFunction == nullptr;
        }

    protected:
        /// @brief Gets the copier function for a function type, if its copyable.
        /// @tparam F Function type.
        /// @return Copier function.
        template <typename F>
        static void* (*getCopier())(const void*)
        {
            constexpr bool IsCopyable = requires(const F& f)
            {
                new F(f);
            };

            if constexpr (IsCopyable)
            {
                return [](const void* function) -> void* { return new F(*static_cast<const F*>(function)); };
            }
            else
            {
                return nullptr;
            }
        }

        void* mFunction{nullptr};
        I mInvoke{};
        void (*mFree)(void*){};
        void* (*mCopy)(const void*){};
    };

    template <typename R, typename... Ts>
    class Function<R(Ts...)> final : public FunctionBase<R (*)(void*, Ts...)>
    {
    public:
        using FunctionBase<R (*)(void*, Ts...)>::FunctionBase;

        /// @brief Constructs.
        /// @tparam F Function type.
        /// @param function Function.
        template <typename F>
        Function(F function)
            : FunctionBase<R (*)(void*, Ts...)>{new F(memory::move(function)),
                                                [](void* function, Ts... args) -> R {
                                                    return (*static_cast<F*>(function))(memory::forward<Ts>(args)...);
                                                },
                                                [](void* function) { delete static_cast<F*>(function); },
                                                FunctionBase<R (*)(void*, Ts...)>::template getCopier<F>()}
        {
        }

        /// @brief Move constructs.
        /// @param other Other.
        Function(Function&& other) noexcept = default;

        /// @brief Copy constructs.
        /// @param other Other.
        Function(const Function& other) = default;

        /// @brief Move assigns.
        /// @param other Other.
        /// @return This.
        Function& operator=(Function&& other) noexcept = default;

        /// @brief Copy assigns.
        /// @param other Other.
        /// @return This.
        Function& operator=(const Function& other) = default;

        /// @brief Invokes the underlying function.
        ///
        /// Must not be null.
        ///
        /// @param args Argument values.
        /// @return Return value.
        R operator()(Ts... args)
        {
            CUBOS_ASSERT(this->mFunction != nullptr, "Function must not be null");
            return this->mInvoke(this->mFunction, memory::forward<Ts>(args)...);
        }
    };

    template <typename R, typename... Ts>
    class Function<R(Ts...) const> final : public FunctionBase<R (*)(const void*, Ts...)>
    {
    public:
        using FunctionBase<R (*)(const void*, Ts...)>::FunctionBase;

        /// @brief Constructs.
        /// @tparam F Function type.
        /// @param function Function.
        template <typename F>
        Function(F function)
            : FunctionBase<R (*)(const void*, Ts...)>{new F(memory::move(function)),
                                                      [](const void* function, Ts... args) -> R {
                                                          return (*static_cast<const F*>(function))(
                                                              memory::forward<Ts>(args)...);
                                                      },
                                                      [](void* function) { delete static_cast<F*>(function); },
                                                      FunctionBase<R (*)(const void*, Ts...)>::template getCopier<F>()}
        {
        }

        /// @brief Move constructs.
        /// @param other Other.
        Function(Function&& other) noexcept = default;

        /// @brief Copy constructs.
        /// @param other Other.
        Function(const Function& other) = default;

        /// @brief Move assigns.
        /// @param other Other.
        /// @return This.
        Function& operator=(Function&& other) noexcept = default;

        /// @brief Copy assigns.
        /// @param other Other.
        /// @return This.
        Function& operator=(const Function& other) = default;

        /// @brief Invokes the underlying function.
        ///
        /// Must not be null.
        ///
        /// @param args Argument values.
        /// @return Return value.
        R operator()(Ts... args) const
        {
            CUBOS_ASSERT(this->mFunction != nullptr, "Function must not be null");
            return this->mInvoke(this->mFunction, memory::forward<Ts>(args)...);
        }
    };
} // namespace cubos::core::memory
