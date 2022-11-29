#ifndef CUBOS_ENGINE_CUBOS_HPP
#define CUBOS_ENGINE_CUBOS_HPP

#include <cubos/core/ecs/dispatcher.hpp>
#include <cubos/core/ecs/world.hpp>

#include <set>
#include <string>

using namespace cubos::core::ecs;

namespace cubos::engine
{
    struct ShouldQuit
    {
        bool value;
    };

    class Cubos
    {
    public:
        Cubos& addPlugin(void (*func)(Cubos&));
        template <typename R, typename... TArgs> Cubos& addResource(TArgs... args);
        template <typename C> Cubos& addComponent();
        template <typename F> Cubos& addSystem(F func, std::string stage);
        template <typename F> Cubos& addStartupSystem(F func, std::string stage);

        void run();

        Cubos();

    private:
        Dispatcher mainDispatcher, startupDispatcher;
        World world;
        std::set<void (*)(Cubos&)> plugins;
    };

    template <typename R, typename... TArgs> Cubos& Cubos::addResource(TArgs... args)
    {
        world.registerResource<R>(args...);
        return *this;
    }

    template <typename C> Cubos& Cubos::addComponent()
    {
        world.registerComponent<C>();
        return *this;
    }

    template <typename F> Cubos& Cubos::addSystem(F func, std::string stage)
    {
        mainDispatcher.addSystem(func, stage);
        return *this;
    }

    template <typename F> Cubos& Cubos::addStartupSystem(F func, std::string stage)
    {
        startupDispatcher.addSystem(func, stage);
        return *this;
    }
} // namespace cubos::engine

#endif // CUBOS_ENGINE_CUBOS_HPP
