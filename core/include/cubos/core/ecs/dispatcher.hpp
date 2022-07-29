#ifndef CUBOS_CORE_ECS_DISPATCHER_HPP
#define CUBOS_CORE_ECS_DISPATCHER_HPP

#include <cubos/core/ecs/system.hpp>

namespace cubos::core::ecs
{
    /// @brief Dispatcher is a class that is used to register systems wrappers and called them when needed.
    class Dispatcher
    {
    public:
        // template to register a system wrapper
        template <typename F> void registerSystem(F system);

        // call all systems
        void callSystems(World& world);

    private:
        // template list of system wrappers pointers
        std::vector<std::unique_ptr<AnySystemWrapper>> systems;
    };

    template <typename F> void Dispatcher::registerSystem(F system)
    {
        systems.emplace_back(std::make_unique<SystemWrapper<F>>(system));
    }
} // namespace cubos::core::ecs
#endif // CUBOS_CORE_ECS_DISPATCHER_HPP