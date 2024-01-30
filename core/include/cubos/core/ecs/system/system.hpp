/// @file
/// @brief Class @ref cubos::core::ecs::SystemWrapper and related types.
///
/// This file is a bit scary, but it's not as bad as it looks. It's mostly template specializations
/// to handle the different types of arguments a system can take.
///
/// @ingroup core-ecs-system

#pragma once

#include <functional>
#include <optional>
#include <typeindex>
#include <unordered_set>
#include <variant>

#include <cubos/core/ecs/system/commands.hpp>
#include <cubos/core/ecs/system/event/reader.hpp>
#include <cubos/core/ecs/system/event/writer.hpp>
#include <cubos/core/ecs/system/query.hpp>
#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    /// @brief Describes a system.
    /// @ingroup core-ecs-system
    struct SystemInfo
    {
        /// @brief Whether the system uses commands or not.
        bool usesCommands;

        /// @brief Whether the system uses the world directly.
        bool usesWorld;

        /// @brief Set of resources the system reads.
        std::unordered_set<std::type_index> resourcesRead;

        /// @brief Set of resources the system writes.
        std::unordered_set<std::type_index> resourcesWritten;

        /// @brief Checks if this system is valid.
        ///
        /// A system may be invalid, if, for example, it both reads and writes the same resource.
        ///
        /// @return Whether the system is valid.
        bool valid() const;

        /// @brief Checks if this system is compatible with another system.
        ///
        /// Two systems are compatible if they can be executed in parallel.
        /// This means that they must not, for example, write to the same resource or component.
        ///
        /// @param other Other system.
        /// @return Whether the systems are compatible.
        bool compatible(const SystemInfo& other) const;
    };

    /// @brief Base class for system wrappers.
    /// @tparam R Return type of the wrapped system.
    /// @ingroup core-ecs-system
    template <typename R>
    class AnySystemWrapper
    {
    public:
        virtual ~AnySystemWrapper() = default;

        /// @brief Constructs.
        /// @param info Information about the wrapped system.
        AnySystemWrapper(SystemInfo&& info);

        /// @brief Prepares the system for being executed on the given world.
        ///
        /// Requires exclusive access to the world and must be called before calling the system.
        ///
        /// @param world World to prepare the system for.
        virtual void prepare(World& world) = 0;

        /// @brief Calls the wrapped system with parameters taken from the given @p world.
        ///
        /// Can only be called after calling @ref prepare() on the same @p world.
        ///
        /// @param world World used by the system.
        /// @param commands Buffer where commands can be submitted to.
        /// @return Return value of the system.
        virtual R call(World& world, CommandBuffer& commands) = 0;

        /// @brief Gets information about the requirements of the system.
        /// @return Information about the system.
        const SystemInfo& info() const;

    private:
        SystemInfo mInfo; ///< Information about the wrapped system.
    };

    namespace impl
    {
        /// @brief Fetches the requested data from a world.
        /// @tparam T System argument type.
        template <typename T>
        struct SystemFetcher
        {
            // This should never be instantiated.
            static_assert(!std::is_same_v<T, T>, "Unknown system argument type.");

            // Placeholders for documentation.

            using Type = char; ///< Type of the fetched data.
            using State = char;

            /// @brief Adds the argument @p T to the given @p info.
            /// @param[out] info System information.
            static void add(SystemInfo& info);

            /// @brief Prepares the argument for being executed on the given @p world.
            /// @param world World to prepare the argument for.
            /// @return State of the argument.
            static State prepare(World& world);

            /// @brief Fetches the data from the given @p world.
            /// @param world World to fetch the data from.
            /// @param commands Buffer where commands can be submitted to.
            /// @param state State of the argument.
            /// @return Fetched data.
            static Type fetch(World& world, CommandBuffer& commands, State& state);

            /// @brief Converts the fetched data into the actual desired argument.
            /// @param fetched Fetched data.
            /// @return Actual argument.
            static T arg(Type&& fetched);
        };

        template <typename R>
        struct SystemFetcher<R&>
        {
            using Type = WriteResource<R>;
            using State = std::monostate;

            static void add(SystemInfo& info);
            static State prepare(World& world);
            static Type fetch(World& world, CommandBuffer& commands, State& state);
            static R& arg(Type&& lock);
        };

        template <typename R>
        struct SystemFetcher<const R&>
        {
            using Type = ReadResource<R>;
            using State = std::monostate;

            static void add(SystemInfo& info);
            static State prepare(World& world);
            static Type fetch(World& world, CommandBuffer& commands, State& state);
            static const R& arg(Type&& lock);
        };

        template <typename... Ts>
        struct SystemFetcher<Query<Ts...>>
        {
            using Type = Query<Ts...>;
            using State = QueryData<Ts...>;

            static void add(SystemInfo& info);
            static State prepare(World& world);
            static Type fetch(World& world, CommandBuffer& commands, State& state);
            static Type arg(Type&& fetched);
        };

        template <>
        struct SystemFetcher<World&>
        {
            using Type = World*;
            using State = std::monostate;

            static void add(SystemInfo& info);
            static State prepare(World& world);
            static Type fetch(World& world, CommandBuffer& commands, State& state);
            static World& arg(Type fetched);
        };

        template <>
        struct SystemFetcher<const World&>
        {
            using Type = const World*;
            using State = std::monostate;

            static void add(SystemInfo& info);
            static State prepare(World& world);
            static Type fetch(World& world, CommandBuffer& commands, State& state);
            static const World& arg(Type fetched);
        };

        template <>
        struct SystemFetcher<Commands>
        {
            using Type = CommandBuffer*;
            using State = std::monostate;

            static void add(SystemInfo& info);
            static State prepare(World& world);
            static Type fetch(World& world, CommandBuffer& commands, State& state);
            static Commands arg(Type fetched);
        };

        template <typename T, unsigned int M>
        struct SystemFetcher<EventReader<T, M>>
        {
            using Type = std::tuple<std::size_t&, ReadResource<EventPipe<T>>>;
            using State = std::size_t; // Number of events read.

            static void add(SystemInfo& info);
            static State prepare(World& world);
            static Type fetch(World& world, CommandBuffer& commands, State& state);
            static EventReader<T, M> arg(Type&& fetched);
        };

        template <typename T>
        struct SystemFetcher<EventWriter<T>>
        {
            using Type = WriteResource<EventPipe<T>>;
            using State = std::monostate;

            static void add(SystemInfo& info);
            static State prepare(World& world);
            static Type fetch(World& world, CommandBuffer& commands, State& state);
            static EventWriter<T> arg(Type&& fetched);
        };

        template <typename... Args>
        struct SystemFetcher<std::tuple<Args...>>
        {
            using Type = std::tuple<typename SystemFetcher<Args>::Type...>;
            using State = std::tuple<typename SystemFetcher<Args>::State...>;

            static void add(SystemInfo& info);
            static State prepare(World& world);
            static Type fetch(World& world, CommandBuffer& commands, State& state);
            static std::tuple<Args...> arg(Type&& fetched);
        };

        /// @brief Template magic used to inspect the arguments of a system.
        template <typename F>
        struct SystemTraits;

        // Specialization for function pointers.

        template <typename R, typename... Args>
        struct SystemTraits<R (*)(Args...)>
        {
            using Return = R;
            using Arguments = std::tuple<Args...>;
            using State = std::tuple<typename SystemFetcher<Args>::State...>;

            static SystemInfo info();
        };

        // Specialization for member functions.

        template <typename R, typename T, typename... Args>
        struct SystemTraits<R (T::*)(Args...)> : SystemTraits<R (*)(Args...)>
        {
        };

        // Specialization for const member functions.

        template <typename R, typename T, typename... Args>
        struct SystemTraits<R (T::*)(Args...) const> : SystemTraits<R (*)(Args...)>
        {
        };

        // Specialization for lambdas and functors, like std::function.

        template <typename F>
        struct SystemTraits : SystemTraits<decltype(&std::remove_reference<F>::type::operator())>
        {
        };

        /// @brief Used to get the index of a type in a tuple.
        ///
        /// Had to use this instead of std::apply due to a bug in MSVC :(
        /// Taken from https://stackoverflow.com/questions/18063451.
        ///
        /// @tparam T Type to get the index of.
        /// @tparam Tuple Tuple to search in.
        template <class T, class Tuple>
        struct Index;

        template <class T, class... Types>
        struct Index<T, std::tuple<T, Types...>>
        {
            static const std::size_t Value = 0;
        };

        template <class T, class U, class... Types>
        struct Index<T, std::tuple<U, Types...>>
        {
            static const std::size_t Value = 1 + Index<T, std::tuple<Types...>>::Value;
        };
    } // namespace impl

    /// @brief Wrapper for a system of type @p F.
    /// @tparam F Type of the system function/method/lambda.
    /// @ingroup core-ecs-system
    template <typename F>
    class SystemWrapper final : public AnySystemWrapper<typename impl::SystemTraits<F>::Return>
    {
    public:
        ~SystemWrapper() override = default;

        /// @brief Constructs.
        /// @param system System to wrap.
        SystemWrapper(F system);

        void prepare(World& world) override;
        typename impl::SystemTraits<F>::Return call(World& world, CommandBuffer& commands) override;

    private:
        friend class Dispatcher;

        F mSystem;                                                   ///< Wrapped system.
        std::optional<typename impl::SystemTraits<F>::State> mState; ///< State of the system.
    };

    // Implementation.

    template <typename R>
    AnySystemWrapper<R>::AnySystemWrapper(SystemInfo&& info)
        : mInfo(std::move(info))
    {
        if (!mInfo.valid())
        {
            CUBOS_CRITICAL("System is invalid - this may happen, if, for example, "
                           "it both reads and writes the same resource");
            abort();
        }
    }

    template <typename R>
    const SystemInfo& AnySystemWrapper<R>::info() const
    {
        return mInfo;
    }

    template <typename R, typename... Args>
    SystemInfo impl::SystemTraits<R (*)(Args...)>::info()
    {
        auto info = SystemInfo();
        info.usesCommands = false;
        info.usesWorld = false;
        impl::SystemFetcher<std::tuple<Args...>>::add(info);
        return info;
    }

    template <typename F>
    SystemWrapper<F>::SystemWrapper(F system)
        : AnySystemWrapper<typename impl::SystemTraits<F>::Return>(impl::SystemTraits<F>::info())
        , mSystem(system)
    {
        // Do nothing.
    }

    template <typename F>
    void SystemWrapper<F>::prepare(World& world)
    {
        using Arguments = typename impl::SystemTraits<F>::Arguments;
        using Fetcher = impl::SystemFetcher<Arguments>;

        if (mState.has_value())
        {
            CUBOS_CRITICAL("System was prepared twice");
            abort();
        }

        mState.emplace(Fetcher::prepare(world));
    }

    template <typename F>
    typename impl::SystemTraits<F>::Return SystemWrapper<F>::call(World& world, CommandBuffer& commands)
    {
        using Arguments = typename impl::SystemTraits<F>::Arguments;
        using Fetcher = impl::SystemFetcher<Arguments>;

        if (!mState.has_value())
        {
            CUBOS_CRITICAL("System was not prepared");
            abort();
        }

        // 1. Fetch the arguments from the world (ReadResource, etc).
        // 2. Convert the fetched data into the actual arguments (e.g: ReadResource<R> to const R&)
        // 3. Pass it into the system.
        auto fetched = Fetcher::fetch(world, commands, mState.value());
        auto args = Fetcher::arg(std::move(fetched));
        return std::apply(mSystem, std::forward<Arguments>(args));
    }

    template <typename R>
    void impl::SystemFetcher<R&>::add(SystemInfo& info)
    {
        info.resourcesWritten.insert(typeid(R));
    }

    template <typename R>
    std::monostate impl::SystemFetcher<R&>::prepare(World& /*unused*/)
    {
        return {};
    }

    template <typename R>
    WriteResource<R> impl::SystemFetcher<R&>::fetch(World& world, CommandBuffer& /*unused*/, State& /*unused*/)
    {
        return world.write<R>();
    }

    template <typename R>
    R& impl::SystemFetcher<R&>::arg(WriteResource<R>&& lock)
    {
        return {lock.get()};
    }

    template <typename R>
    void impl::SystemFetcher<const R&>::add(SystemInfo& info)
    {
        info.resourcesRead.insert(typeid(R));
    }

    template <typename R>
    std::monostate impl::SystemFetcher<const R&>::prepare(World& /*unused*/)
    {
        return {};
    }

    template <typename R>
    ReadResource<R> impl::SystemFetcher<const R&>::fetch(World& world, CommandBuffer& /*unused*/, State& /*unused*/)
    {
        return world.read<R>();
    }

    template <typename R>
    const R& impl::SystemFetcher<const R&>::arg(ReadResource<R>&& lock)
    {
        return {lock.get()};
    }

    template <typename... Ts>
    void impl::SystemFetcher<Query<Ts...>>::add(SystemInfo& info)
    {
        // TODO: implement this
        (void)info;
    }

    template <typename... Ts>
    QueryData<Ts...> impl::SystemFetcher<Query<Ts...>>::prepare(World& world)
    {
        return {world, {}};
    }

    template <typename... Ts>
    Query<Ts...> impl::SystemFetcher<Query<Ts...>>::fetch(World& /*unused*/, CommandBuffer& /*unused*/, State& state)
    {
        state.update();
        return Query<Ts...>(state.view());
    }

    template <typename... ComponentTypes>
    Query<ComponentTypes...> impl::SystemFetcher<Query<ComponentTypes...>>::arg(Query<ComponentTypes...>&& fetched)
    {
        return std::move(fetched);
    }

    inline void impl::SystemFetcher<World&>::add(SystemInfo& info)
    {
        info.usesWorld = true;
    }

    inline std::monostate impl::SystemFetcher<World&>::prepare(World& /*unused*/)
    {
        return {};
    }

    inline World* impl::SystemFetcher<World&>::fetch(World& world, CommandBuffer& /*unused*/, State& /*unused*/)
    {
        return &world;
    }

    inline World& impl::SystemFetcher<World&>::arg(World* fetched)
    {
        return {*fetched};
    }

    inline void impl::SystemFetcher<const World&>::add(SystemInfo& info)
    {
        info.usesWorld = true;
    }

    inline std::monostate impl::SystemFetcher<const World&>::prepare(World& /*unused*/)
    {
        return {};
    }

    inline const World* impl::SystemFetcher<const World&>::fetch(World& world, CommandBuffer& /*unused*/,
                                                                 State& /*unused*/)
    {
        return &world;
    }

    inline const World& impl::SystemFetcher<const World&>::arg(const World* fetched)
    {
        return {*fetched};
    }

    inline void impl::SystemFetcher<Commands>::add(SystemInfo& info)
    {
        info.usesCommands = true;
    }

    inline std::monostate impl::SystemFetcher<Commands>::prepare(World& /*unused*/)
    {
        return {};
    }

    inline CommandBuffer* impl::SystemFetcher<Commands>::fetch(World& /*unused*/, CommandBuffer& commands,
                                                               State& /*unused*/)
    {
        return &commands;
    }

    inline Commands impl::SystemFetcher<Commands>::arg(CommandBuffer* fetched)
    {
        return {*fetched};
    }

    template <typename T, unsigned int M>
    void impl::SystemFetcher<EventReader<T, M>>::add(SystemInfo& info)
    {
        info.resourcesRead.insert(typeid(T));
    }

    template <typename T, unsigned int M>
    std::size_t impl::SystemFetcher<EventReader<T, M>>::prepare(World& world)
    {
        world.write<EventPipe<T>>().get().addReader();
        return 0; // Initially we haven't read any events.
    }

    template <typename T, unsigned int M>
    std::tuple<std::size_t&, ReadResource<EventPipe<T>>> impl::SystemFetcher<EventReader<T, M>>::fetch(
        World& world, CommandBuffer& /*unused*/, State& state)
    {
        return std::forward_as_tuple(state, world.read<EventPipe<T>>());
    }

    template <typename T, unsigned int M>
    EventReader<T, M> impl::SystemFetcher<EventReader<T, M>>::arg(
        std::tuple<std::size_t&, ReadResource<EventPipe<T>>>&& fetched)
    {
        return EventReader<T>(std::get<1>(fetched).get(), std::get<0>(fetched));
    }

    template <typename T>
    void impl::SystemFetcher<EventWriter<T>>::add(SystemInfo& info)
    {
        info.resourcesWritten.insert(typeid(T));
    }

    template <typename T>
    std::monostate impl::SystemFetcher<EventWriter<T>>::prepare(World& /*unused*/)
    {
        return {};
    }

    template <typename T>
    WriteResource<EventPipe<T>> impl::SystemFetcher<EventWriter<T>>::fetch(World& world, CommandBuffer& /*unused*/,
                                                                           State& /*unused*/)
    {
        return world.write<EventPipe<T>>();
    }

    template <typename T>
    EventWriter<T> impl::SystemFetcher<EventWriter<T>>::arg(WriteResource<EventPipe<T>>&& fetched)
    {
        return EventWriter<T>(fetched.get());
    }

    template <typename... Args>
    void impl::SystemFetcher<std::tuple<Args...>>::add(SystemInfo& info)
    {
        ([&]() { impl::SystemFetcher<Args>::add(info); }(), ...);
    }

    template <typename... Args>
    std::tuple<typename impl::SystemFetcher<Args>::State...> impl::SystemFetcher<std::tuple<Args...>>::prepare(
        World& world)
    {
        return std::forward_as_tuple(impl::SystemFetcher<Args>::prepare(world)...);
    }

    template <typename... Args>
    std::tuple<typename impl::SystemFetcher<Args>::Type...> impl::SystemFetcher<std::tuple<Args...>>::fetch(
        World& world, CommandBuffer& commands, State& state)
    {
        return std::make_tuple(impl::SystemFetcher<Args>::fetch(
            world, commands, std::get<impl::Index<Args, std::tuple<Args...>>::Value>(state))...);
    }

    template <typename... Args>
    std::tuple<Args...> impl::SystemFetcher<std::tuple<Args...>>::arg(
        std::tuple<typename impl::SystemFetcher<Args>::Type...>&& fetched)
    {
        return std::forward_as_tuple(
            impl::SystemFetcher<Args>::arg(std::move(std::get<typename impl::SystemFetcher<Args>::Type>(fetched)))...);
    }
} // namespace cubos::core::ecs
