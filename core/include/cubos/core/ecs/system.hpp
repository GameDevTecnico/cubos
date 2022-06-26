#ifndef CUBOS_CORE_ECS_SYSTEM_HPP
#define CUBOS_CORE_ECS_SYSTEM_HPP

#include <cubos/core/ecs/world.hpp>

#include <functional>

namespace cubos::core::ecs
{
    /// Base class for all systems.
    class System
    {
    public:
        System() = default;
        virtual ~System() = default;

        /// Called when the system is added to the world.
        /// You can use this, for example, to create entities.
        /// @param world The world the system is added to.
        virtual void init(World& world);

        /// Called every frame of the engine.
        /// The system logic should be implemented in this function.
        ///
        /// @details For example, if you want to add the velocity of every entity to its
        /// position every frame, you would do it in this function.
        /// @param world The world the system is added to.
        virtual void update(World& world);
    };

    /// Base class for system wrappers.
    class AnySystemWrapper
    {
    public:
        AnySystemWrapper() = default;
        virtual ~AnySystemWrapper() = default;

        /// Calls the wrapped system with parameters taken from the given world.
        /// @param world The world used by the system.
        virtual void call(const World& world) const = 0;
    };

    /// A system wrapper for a system which takes some arguments.
    template <typename F> class SystemWrapper final : public AnySystemWrapper
    {
    public:
        /// @param system The system to wrap.
        SystemWrapper(F system);
        virtual ~SystemWrapper() override = default;

        /// @see AnySystemWrapper::call
        virtual void call(const World& world) const override;

    private:
        F system; ///< The wrapped system.
    };

    /// This namespace contains functions used internally by the implementation
    /// of the ECS.
    namespace impl
    {
        /// Fetches the requested type from a world.
        template <typename T> struct Fetcher
        {
            // This should never be instantiated.
            static_assert(!std::is_same_v<T, T>, "Unknown system argument type.");
        };

        template <typename R> struct Fetcher<R&>
        {
            using Type = WriteResource<R>;

            /// @param world The world to fetch from.
            /// @returns The requested resource write lock.
            static WriteResource<R> fetch(const World& world);

            /// @param lock The resource lock to get the reference from.
            /// @returns The requested resource reference.
            static R& arg(WriteResource<R>&& lock);
        };

        template <typename R> struct Fetcher<const R&>
        {
            using Type = ReadResource<R>;

            /// @param world The world to fetch from.
            /// @returns The requested resource read lock.
            static ReadResource<R> fetch(const World& world);

            /// @param lock The resource lock to get the reference from.
            /// @returns The requested resource reference.
            static const R& arg(ReadResource<R>&& lock);
        };

        template <typename... Args> struct Fetcher<std::tuple<Args...>>
        {
            using Type = std::tuple<typename Fetcher<Args>::Type...>;

            /// @param world The world to fetch from.
            /// @returns The requested arguments fetch result.
            static Type fetch(const World& world);

            /// @param fetched The requested arguments fetch result.
            /// @returns The requested arguments.
            static std::tuple<Args...> arg(Type&& fetched);
        };

        /// Template magic used to inspect the arguments of a system.
        template <typename F> struct SystemTraits;

        /// Specialization for function pointers.
        template <typename... Args> struct SystemTraits<void (*)(Args...)>
        {
            using Arguments = std::tuple<Args...>;
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
        template <typename F> struct SystemTraits : SystemTraits<decltype(&std::remove_reference<F>::type::operator())>
        {
        };
    } // namespace impl

    // Implementation.

    template <typename F> SystemWrapper<F>::SystemWrapper(F system) : system(system)
    {
        // Do nothing.
    }

    template <typename F> void SystemWrapper<F>::call(const World& world) const
    {
        using Fetcher = impl::Fetcher<typename impl::SystemTraits<F>::Arguments>;

        // 1. Fetch the arguments from the world (ReadResource, etc).
        // 2. Convert the fetched data into the actual arguments (e.g: ReadResource<R> to const R&)
        // 3. Pass it into the system.
        auto fetched = Fetcher::fetch(world);
        auto args = Fetcher::arg(std::move(fetched));
        std::apply(this->system, args);
    }

    template <typename R> WriteResource<R> impl::Fetcher<R&>::fetch(const World& world)
    {
        return world.writeResource<R>();
    }

    template <typename R> R& impl::Fetcher<R&>::arg(WriteResource<R>&& lock)
    {
        return lock.get();
    }

    template <typename R> ReadResource<R> impl::Fetcher<const R&>::fetch(const World& world)
    {
        return world.readResource<R>();
    }

    template <typename R> const R& impl::Fetcher<const R&>::arg(ReadResource<R>&& lock)
    {
        return lock.get();
    }

    template <typename... Args>
    std::tuple<typename impl::Fetcher<Args>::Type...> impl::Fetcher<std::tuple<Args...>>::fetch(const World& world)
    {
        return std::make_tuple(impl::Fetcher<Args>::fetch(world)...);
    }

    template <typename... Args>
    std::tuple<Args...> impl::Fetcher<std::tuple<Args...>>::arg(
        std::tuple<typename impl::Fetcher<Args>::Type...>&& fetched)
    {
        return std::forward_as_tuple(
            impl::Fetcher<Args>::arg(std::move(std::get<typename impl::Fetcher<Args>::Type>(fetched)))...);
    }
} // namespace cubos::core::ecs
#endif // CUBOS_CORE_ECS_SYSTEM_HPP
