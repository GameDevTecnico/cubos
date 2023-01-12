#ifndef CUBOS_CORE_ECS_SYSTEM_HPP
#define CUBOS_CORE_ECS_SYSTEM_HPP

#include <cubos/core/ecs/world.hpp>
#include <cubos/core/ecs/debug.hpp>
#include <cubos/core/ecs/query.hpp>
#include <cubos/core/ecs/commands.hpp>

#include <functional>
#include <unordered_set>
#include <typeindex>

namespace cubos::core::ecs
{
    /// Contains information about a system.
    struct SystemInfo
    {
        /// Whether the system uses commands or not.
        bool usesCommands;

        /// Whether the system queries debug information or not.
        bool usesDebug;

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
    class AnySystemWrapper
    {
    public:
        AnySystemWrapper(SystemInfo&& info);
        virtual ~AnySystemWrapper() = default;

        /// Calls the wrapped system with parameters taken from the given world.
        /// @param world The world used by the system.
        /// @param commands The commands object used by the system.
        virtual void call(const World& world, Commands& commands) const = 0;

        /// Gets information about the requirements of the system.
        const SystemInfo& info() const;

    private:
        SystemInfo m_info; ///< Information about the wrapped system.
    };

    /// A system wrapper for a system which takes some arguments.
    template <typename F>
    class SystemWrapper final : public AnySystemWrapper
    {
    public:
        /// @param system The system to wrap.
        SystemWrapper(F system);
        virtual ~SystemWrapper() override = default;

        /// @see AnySystemWrapper::call
        virtual void call(const World& world, Commands& commands) const override;

    private:
        F system; ///< The wrapped system.
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

            /// Adds the resource to the system info structure.
            /// @param info The system info structure to add to.
            static void add(SystemInfo& info);

            /// @param world The world to fetch from.
            /// @param commands The commands object.
            /// @returns The requested resource write lock.
            static WriteResource<R> fetch(const World& world, Commands& commands);

            /// @param lock The resource lock to get the reference from.
            /// @returns The requested resource reference.
            static R& arg(const World& world, WriteResource<R>&& lock);
        };

        template <typename R>
        struct SystemFetcher<const R&>
        {
            using Type = ReadResource<R>;

            /// Adds the resource to the system info structure.
            /// @param info The system info structure to add to.
            static void add(SystemInfo& info);

            /// @param world The world to fetch from.
            /// @param commands The commands object.
            /// @returns The requested resource read lock.
            static ReadResource<R> fetch(const World& world, Commands& commands);

            /// @param lock The resource lock to get the reference from.
            /// @returns The requested resource reference.
            static const R& arg(const World& world, ReadResource<R>&& lock);
        };

        template <typename... ComponentTypes>
        struct SystemFetcher<Query<ComponentTypes...>>
        {
            using Type = Query<ComponentTypes...>;

            /// Adds the query to the system info structure.
            /// @param info The system info structure to add to.
            static void add(SystemInfo& info);

            /// @param world The world to query from.
            /// @param commands The commands object.
            /// @returns The requested query data.
            static Type fetch(const World& world, Commands& commands);

            /// @param fetched The fetched query.
            /// @returns The query.
            static Type arg(const World& world, Type&& fetched);
        };

        template <>
        struct SystemFetcher<Debug>
        {
            using Type = Debug;

            /// Adds the debug query to the system info structure.
            /// @param info The system info structure to add to.
            static void add(SystemInfo& info);

            /// @param world The world to query from.
            /// @param commands The commands object.
            /// @returns The requested query data.
            static Type fetch(const World& world, Commands& commands);

            /// @param fetched The fetched query.
            /// @returns The query.
            static Type arg(const World& world, Type&& fetched);
        };

        template <>
        struct SystemFetcher<Commands&>
        {
            using Type = Commands*;

            /// Adds the commands to the system info structure.
            /// @param info The system info structure to add to.
            static void add(SystemInfo& info);

            /// @param world The world to query from.
            /// @param commands The commands object.
            /// @returns The requested query data.
            static Commands* fetch(const World& world, Commands& commands);

            /// @param fetched The fetched query.
            /// @returns The query.
            static Commands& arg(const World& world, Commands* fetched);
        };

        template <typename... Args>
        struct SystemFetcher<std::tuple<Args...>>
        {
            using Type = std::tuple<typename SystemFetcher<Args>::Type...>;

            /// Adds the arguments to the system info structure.
            /// @param info The system info structure to add to.
            static void add(SystemInfo& info);

            /// @param world The world to fetch from.
            /// @param commands The commands object.
            /// @returns The requested arguments fetch result.
            static Type fetch(const World& world, Commands& commands);

            /// @param fetched The requested arguments fetch result.
            /// @returns The requested arguments.
            static std::tuple<Args...> arg(const World& world, Type&& fetched);
        };

        /// Template magic used to inspect the arguments of a system.
        template <typename F>
        struct SystemTraits;

        /// Specialization for function pointers.
        template <typename... Args>
        struct SystemTraits<void (*)(Args...)>
        {
            using Arguments = std::tuple<Args...>;

            /// Gets information about the system.
            static SystemInfo info();
        };

        /// Specialization for member functions.
        template <typename T, typename... Args>
        struct SystemTraits<void (T::*)(Args...)> : SystemTraits<void (*)(Args...)>
        {
        };

        /// Specialization for const member functions.
        template <typename T, typename... Args>
        struct SystemTraits<void (T::*)(Args...) const> : SystemTraits<void (*)(Args...)>
        {
        };

        /// Specialization for lambdas and functors, like std::function.
        template <typename F>
        struct SystemTraits : SystemTraits<decltype(&std::remove_reference<F>::type::operator())>
        {
        };
    } // namespace impl

    // Implementation.

    template <typename... Args>
    SystemInfo impl::SystemTraits<void (*)(Args...)>::info()
    {
        auto info = SystemInfo();
        info.usesCommands = false;
        info.usesDebug = false;
        impl::SystemFetcher<std::tuple<Args...>>::add(info);
        return info;
    }

    template <typename F>
    SystemWrapper<F>::SystemWrapper(F system) : AnySystemWrapper(impl::SystemTraits<F>::info()), system(system)
    {
        // Do nothing.
    }

    template <typename F>
    void SystemWrapper<F>::call(const World& world, Commands& commands) const
    {
        using Arguments = typename impl::SystemTraits<F>::Arguments;
        using Fetcher = impl::SystemFetcher<Arguments>;

        // 1. Fetch the arguments from the world (ReadResource, etc).
        // 2. Convert the fetched data into the actual arguments (e.g: ReadResource<R> to const R&)
        // 3. Pass it into the system.
        auto fetched = Fetcher::fetch(world, commands);
        auto args = Fetcher::arg(world, std::move(fetched));
        std::apply(this->system, std::forward<Arguments>(args));
    }

    template <typename R>
    void impl::SystemFetcher<R&>::add(SystemInfo& info)
    {
        info.resourcesWritten.insert(typeid(R));
    }

    template <typename R>
    WriteResource<R> impl::SystemFetcher<R&>::fetch(const World& world, Commands&)
    {
        return world.write<R>();
    }

    template <typename R>
    R& impl::SystemFetcher<R&>::arg(const World& world, WriteResource<R>&& lock)
    {
        return lock.get();
    }

    template <typename R>
    void impl::SystemFetcher<const R&>::add(SystemInfo& info)
    {
        info.resourcesRead.insert(typeid(R));
    }

    template <typename R>
    ReadResource<R> impl::SystemFetcher<const R&>::fetch(const World& world, Commands&)
    {
        return world.read<R>();
    }

    template <typename R>
    const R& impl::SystemFetcher<const R&>::arg(const World& world, ReadResource<R>&& lock)
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
    Query<ComponentTypes...> impl::SystemFetcher<Query<ComponentTypes...>>::fetch(const World& world, Commands&)
    {
        return Query<ComponentTypes...>(world);
    }

    template <typename... ComponentTypes>
    Query<ComponentTypes...> impl::SystemFetcher<Query<ComponentTypes...>>::arg(const World& world,
                                                                                Query<ComponentTypes...>&& fetched)
    {
        return std::move(fetched);
    }

    inline void impl::SystemFetcher<Debug>::add(SystemInfo& info)
    {
        info.usesDebug = true;
    }

    inline Debug impl::SystemFetcher<Debug>::fetch(const World& world, Commands&)
    {
        // Albeit incredibly cursed and ugly, this const_cast is safe since we're certain this system
        // is the only one running in this stage.
        return Debug(const_cast<World&>(world));
    }

    inline Debug impl::SystemFetcher<Debug>::arg(const World& world, Debug&& fetched)
    {
        return std::move(fetched);
    }

    inline void impl::SystemFetcher<Commands&>::add(SystemInfo& info)
    {
        info.usesCommands = true;
    }

    inline Commands* impl::SystemFetcher<Commands&>::fetch(const World&, Commands& commands)
    {
        return &commands;
    }

    inline Commands& impl::SystemFetcher<Commands&>::arg(const World&, Commands* fetched)
    {
        return *fetched;
    }

    template <typename... Args>
    void impl::SystemFetcher<std::tuple<Args...>>::add(SystemInfo& info)
    {
        ([&]() { impl::SystemFetcher<Args>::add(info); }(), ...);
    }

    template <typename... Args>
    std::tuple<typename impl::SystemFetcher<Args>::Type...> impl::SystemFetcher<std::tuple<Args...>>::fetch(
        const World& world, Commands& commands)
    {
        return std::make_tuple(impl::SystemFetcher<Args>::fetch(world, commands)...);
    }

    template <typename... Args>
    std::tuple<Args...> impl::SystemFetcher<std::tuple<Args...>>::arg(
        const World& world, std::tuple<typename impl::SystemFetcher<Args>::Type...>&& fetched)
    {
        return std::forward_as_tuple(impl::SystemFetcher<Args>::arg(
            world, std::move(std::get<typename impl::SystemFetcher<Args>::Type>(fetched)))...);
    }
} // namespace cubos::core::ecs
#endif // CUBOS_CORE_ECS_SYSTEM_HPP
