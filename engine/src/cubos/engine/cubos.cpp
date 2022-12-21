#include <cubos/core/log.hpp>
#include <cubos/core/settings.hpp>
#include <cubos/core/ecs/commands.hpp>

#include <cubos/engine/cubos.hpp>

using namespace cubos::engine;

Cubos& Cubos::addPlugin(void (*func)(Cubos&))
{
    if (!plugins.contains(func))
    {
        func(*this);
        plugins.insert(func);
    }
    else
    {
        CUBOS_TRACE("Plugin was already registered!");
    }
    return *this;
}

Cubos& Cubos::tag(const std::string& tag)
{
    mainDispatcher.addTag(tag);

    return *this;
}

Cubos& Cubos::startupTag(const std::string& tag)
{
    startupDispatcher.addTag(tag);

    return *this;
}

Cubos& Cubos::tagged(const std::string& tag)
{
    if(isStartup)
    {
        startupDispatcher.setTag(tag);
    }
    else
    {
        mainDispatcher.setTag(tag);
    }

    return *this;
}

Cubos& Cubos::after(const std::string& tag)
{
    if(isStartup)
    {
        startupDispatcher.setAfter(tag);
    }
    else
    {
        mainDispatcher.setAfter(tag);
    }

    return *this;
}

Cubos& Cubos::before(const std::string& tag)
{
    if(isStartup)
    {
        startupDispatcher.setBefore(tag);
    }
    else
    {
        mainDispatcher.setBefore(tag);
    }

    return *this;
}

Cubos::Cubos()
{
    core::initializeLogger();

    addResource<ShouldQuit>(true);
    addResource<cubos::core::Settings>();

    isStartup = false;
}

void Cubos::run()
{
    plugins.clear();

    // Compile execution chain
    startupDispatcher.compileChain();
    mainDispatcher.compileChain();

    cubos::core::ecs::Commands cmds(world);

    startupDispatcher.callSystems(world, cmds);

    while (!world.read<ShouldQuit>().get().value)
    {
        mainDispatcher.callSystems(world, cmds);
    }
}
