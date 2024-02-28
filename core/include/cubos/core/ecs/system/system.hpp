/// @file
/// @brief Class @ref cubos::core::ecs::System.
/// @ingroup core-ecs-system

#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <typeindex>
#include <unordered_set>
#include <variant>

#include <cubos/core/ecs/system/access.hpp>
#include <cubos/core/ecs/system/arguments/commands.hpp>
#include <cubos/core/ecs/system/arguments/event/reader.hpp>
#include <cubos/core/ecs/system/arguments/event/writer.hpp>
#include <cubos/core/ecs/system/arguments/query.hpp>
#include <cubos/core/ecs/system/arguments/resources.hpp>
#include <cubos/core/ecs/system/arguments/world.hpp>
#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    /// @brief Holds a system with a return type @p T.
    /// @tparam T System return type.
    /// @ingroup core-ecs-system
    template <typename T>
    class System
    {
    public:
        /// @brief Creates a new system for the given world with the given function and options.
        ///
        /// The first option is applied to the first argument which requests it, and the same for subsequent options.
        /// If there are more options than arguments which request them, aborts. Otherwise, if there are more arguments
        /// requesting options than supplied options, the remaining arguments are assigned default-value options.
        ///
        /// @param world World the system will access.
        /// @param function System function, which should return @p T.
        /// @param options Options for the system arguments.
        static System make(World& world, auto function, const std::vector<SystemOptions>& options = {})
        {
            using Function = typename SystemTraits<decltype(function)>::Function;
            return {std::make_unique<Function>(world, std::move(function), options)};
        }

        /// @brief Move constructs.
        /// @param system Other system.
        System(System&& system) noexcept = default;

        /// @brief Runs the system.
        /// @param ctx Context.
        /// @return Return value.
        T run(const SystemContext& ctx)
        {
            return mFunction->call(ctx);
        }

        /// @brief Gets a reference to the access patterns of the system.
        /// @return Access patterns.
        const SystemAccess& access() const
        {
            return mFunction->access();
        }

    private:
        /// @brief Base class for the stored system function type.
        class AnySystemFunction;

        /// @brief Specific class for the stored system function type with specific arguments.
        /// @tparam As Argument types.
        template <typename F, typename... As>
        class SystemFunction; // NOLINT(cppcoreguidelines-virtual-class-destructor)

        /// @brief Used to get a @ref SystemFunction from any functional type.
        /// @tparam F Original function type.
        /// @tparam Ft Transformed function type, initially = F.
        template <typename F, typename Ft = F>
        struct SystemTraits;

        /// @brief Constructs.
        /// @param function System function.
        System(std::unique_ptr<AnySystemFunction> function)
            : mFunction{std::move(function)}
        {
        }

        std::unique_ptr<AnySystemFunction> mFunction;
    };

    template <typename T>
    class System<T>::AnySystemFunction
    {
    public:
        virtual ~AnySystemFunction() = default;

        /// @brief Calls the system function.
        /// @param ctx Context.
        /// @return Return value.
        virtual T call(const SystemContext& ctx) = 0;

        /// @brief Gets a reference to the access patterns of the system.
        /// @return Access patterns.
        const SystemAccess& access() const
        {
            return mAccess;
        }

        /// @copydoc access()
        SystemAccess& access()
        {
            return mAccess;
        }

    private:
        SystemAccess mAccess;
    };

    // The warning being disabled seems to be an error in clang-tidy, as described in
    // https://github.com/llvm/llvm-project/issues/51759
    // NOLINTBEGIN(cppcoreguidelines-virtual-class-destructor)
    template <typename T>
    template <typename F, typename... As>
    class System<T>::SystemFunction : public AnySystemFunction
    {
    public:
        ~SystemFunction() override
        {
            delete mFetchers;
        }

        /// @brief Constructs.
        /// @param world World.
        /// @param function Function.
        /// @param options System options for each argument.
        SystemFunction(World& world, F function, const std::vector<SystemOptions>& options)
            : mFunction{std::move(function)}
        {
            // Get a vector with with options for each of the arguments. This step is necessary as the user only
            // supplies options for the arguments which need them.
            std::vector<SystemOptions> allOptions{};
            std::size_t nextOption = 0;
            if constexpr (sizeof...(As) > 0)
            {
                for (bool consume : {SystemFetcher<As>::ConsumesOptions...})
                {
                    if (consume && nextOption < options.size())
                    {
                        // Argument requests an option and the user supplied one.
                        allOptions.emplace_back(options[nextOption++]);
                    }
                    else
                    {
                        // Either the argument doesn't request an argument, or the user didn't supply enough options. In
                        // any case, we set the option to the default value.
                        allOptions.emplace_back();
                    }
                }
            }

            CUBOS_ASSERT(nextOption == options.size(),
                         "{} options were passed to the system but only {} of its arguments are configurable",
                         options.size(), nextOption);

            // Initialize the fetchers with a templated functor which receives a sequence of indices (at compile time)
            // and initializes each fetcher for the options with the corresponding index.
            auto initFetchers = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return new std::tuple<SystemFetcher<As>...>(SystemFetcher<As>(world, allOptions[Is])...);
            };

            // Call the above functor with a sequence of indices from 0 to the number of fetchers.
            mFetchers = initFetchers(std::index_sequence_for<As...>{});

            // Extract the access patterns from each of the fetchers.
            std::apply([&](auto&&... fetchers) { (fetchers.analyze(this->access()), ...); }, *mFetchers);
        }

        T call(const SystemContext& ctx) final
        {
            return std::apply([&](auto&&... fetchers) { return mFunction(fetchers.fetch(ctx)...); }, *mFetchers);
        }

    private:
        F mFunction;
        std::tuple<SystemFetcher<As>...>* mFetchers;
    };
    // NOLINTEND(cppcoreguidelines-virtual-class-destructor)

    template <typename T>
    template <typename F, typename... As>
    struct System<T>::SystemTraits<F, T (*)(As...)>
    {
        using Function = SystemFunction<F, As...>;
    };

    template <typename T>
    template <typename F, typename O, typename... As>
    struct System<T>::SystemTraits<F, T (O::*)(As...)>
    {
        using Function = SystemFunction<F, As...>;
    };

    template <typename T>
    template <typename F, typename O, typename... As>
    struct System<T>::SystemTraits<F, T (O::*)(As...) const>
    {
        using Function = SystemFunction<F, As...>;
    };

    template <typename T>
    template <typename F, typename Ft>
    struct System<T>::SystemTraits : SystemTraits<F, decltype(&std::remove_reference_t<Ft>::operator())>
    {
    };
} // namespace cubos::core::ecs
