#ifndef CUBOS_CORE_ECS_SYSTEM_HPP
#define CUBOS_CORE_ECS_SYSTEM_HPP

#include <functional>
#include <typeindex>
#include <unordered_set>

#include <cubos/core/ecs/commands.hpp>
#include <cubos/core/ecs/event_reader.hpp>
#include <cubos/core/ecs/event_writer.hpp>
#include <cubos/core/ecs/query.hpp>
#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    /// Contains information about a system.
    struct SystemInfo
    {
        /// Whether the system uses commands or not.
        bool usesCommands;

        /// Whether the system uses the world directly.
        bool usesWorld;

        /// The type set of resources the system reads.
        std::unordered_set<std::type_index> resourcesRead;

        /// The type set of resources the system writes.
        std::unordered_set<std::type_index> resourcesWritten;

        /// The type set of components the system reads.
        std::unordered_set<std::type_index> componentsRead;

        /// The type set of components the system writes.
        std::unordered_set<std::type_index> componentsWritten;

        /// Checks if this system is valid.
        /// A system may be invalid, if, for example, it both reads and writes the same resource.
        /// @returns True if the system is valid, false otherwise.
        bool valid() const;

        /// Checks if this system is compatible with another system.
        /// @param other The other system info to check compatibility with.
        /// @returns True if the systems are compatible, false otherwise.
        bool compatible(const SystemInfo& other) const;
    };

    /// Base class for system wrappers.
    template <typename R>
    class AnySystemWrapper
    {
    public:
        AnySystemWrapper(SystemInfo&& info);
        virtual ~AnySystemWrapper() = default;

        /// Prepares the system for being executed on the given world.
        /// Requires exclusive access to the world, must be called before calling the system.
        /// @param world The world to prepare the system for.
        virtual void prepare(World& world) = 0;

        /// Calls the wrapped system with parameters taken from the given world.
        /// @param world The world used by the system.
        /// @param commands The commands object used by the system.
        /// @returns The return value of the system.
        virtual R call(World& world, CommandBuffer& commands) = 0;

        /// Gets information about the requirements of the system.
        const SystemInfo& info() const;

    private:
        SystemInfo mInfo; ///< Information about the wrapped system.
    };

    /// This namespace contains functions used internally by the implementation
    /// of the ECS.
    namespace impl
    {
        /// Fetches the requested type from a world.
        template <typename T>
        struct SystemFetcher
        {
            // This should never be instantiated.
            static_assert(!std::is_same_v<T, T>, "Unknown system argument type.");
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

        template <typename... ComponentTypes>
        struct SystemFetcher<Query<ComponentTypes...>>
        {
            using Type = Query<ComponentTypes...>;
            using State = std::monostate;

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

        /// Template magic used to inspect the arguments of a system.
        template <typename F>
        struct SystemTraits;

        /// Specialization for function pointers.
        template <typename R, typename... Args>
        struct SystemTraits<R (*)(Args...)>
        {
            using Return = R;
            using Arguments = std::tuple<Args...>;
            using State = std::tuple<typename SystemFetcher<Args>::State...>;

            /// Gets information about the system.
            static SystemInfo info();
        };

        /// Specialization for member functions.
        template <typename R, typename T, typename... Args>
        struct SystemTraits<R (T::*)(Args...)> : SystemTraits<R (*)(Args...)>
        {
        };

        /// Specialization for const member functions.
        template <typename R, typename T, typename... Args>
        struct SystemTraits<R (T::*)(Args...) const> : SystemTraits<R (*)(Args...)>
        {
        };

        /// Specialization for lambdas and functors, like std::function.
        template <typename F>
        struct SystemTraits : SystemTraits<decltype(&std::remove_reference<F>::type::operator())>
        {
        };

        /// Used to get the index of a type in a tuple.
        /// Had to use this instead of std::apply due to a bug in MSVC :(
        /// Taken from https://stackoverflow.com/questions/18063451.
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

    /// A system wrapper for a system which takes some arguments.
    /// @tparam F The type of the system function/method/lambda.
    template <typename F>
    class SystemWrapper final : public AnySystemWrapper<typename impl::SystemTraits<F>::Return>
    {
    public:
        friend class Dispatcher;
        /// @param system The system to wrap.
        SystemWrapper(F system);
        ~SystemWrapper() override = default;

        /// @see AnySystemWrapper::prepare
        void prepare(World& world) override;
        /// @see AnySystemWrapper::call
        typename impl::SystemTraits<F>::Return call(World& world, CommandBuffer& commands) override;

    private:
        F mSystem;                                                   ///< The wrapped system.
        std::optional<typename impl::SystemTraits<F>::State> mState; ///< The state of the system.
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

        mState = Fetcher::prepare(world);
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
        return lock.get();
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
        return lock.get();
    }

    template <typename... ComponentTypes>
    void impl::SystemFetcher<Query<ComponentTypes...>>::add(SystemInfo& info)
    {
        auto queryInfo = Query<ComponentTypes...>::info();

        for (auto& comp : queryInfo.read)
        {
            info.componentsRead.insert(comp);
        }

        for (auto& comp : queryInfo.written)
        {
            info.componentsWritten.insert(comp);
        }
    }

    template <typename... ComponentTypes>
    std::monostate impl::SystemFetcher<Query<ComponentTypes...>>::prepare(World& /*unused*/)
    {
        return {};
    }

    template <typename... ComponentTypes>
    Query<ComponentTypes...> impl::SystemFetcher<Query<ComponentTypes...>>::fetch(World& world,
                                                                                  CommandBuffer& /*unused*/,
                                                                                  State& /*unused*/)
    {
        return Query<ComponentTypes...>(world);
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
        return *fetched;
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
#endif // CUBOS_CORE_ECS_SYSTEM_HPP
