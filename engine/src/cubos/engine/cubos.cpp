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

Cubos& Cubos::after(const std::string& tag)
{
    if (!currentTag && !currentSystem)
    {
        CUBOS_ERROR("No tag or system currently selected! (on after({}))", tag);
        abort();
    }

    if (currentTag)
    {
        currentTag->after.push_back(tag);
    }
    else if (currentSystem)
    {
        currentSystem->after.push_back(tag);
    }

    return *this;
}

Cubos& Cubos::before(const std::string& tag)
{
    if (!currentTag && !currentSystem)
    {
        CUBOS_ERROR("No tag or system currently selected! (on before({}))", tag);
        abort();
    }

    if (currentTag)
    {
        currentTag->before.push_back(tag);
    }
    else if (currentSystem)
    {
        currentSystem->before.push_back(tag);
    }

    return *this;
}

Cubos::Cubos()
{
    core::initializeLogger();

    addResource<ShouldQuit>(true);
    addResource<cubos::core::Settings>();
}

void Cubos::compileChain()
{
    compileStartupDispatcher();
    compileMainDispatcher();
}

void Cubos::compileStartupDispatcher()
{

}

void Cubos::compileMainDispatcher()
{

}

void Cubos::run()
{
    plugins.clear();

    // Compile execution chain
    compileChain();

    cubos::core::ecs::Commands cmds(world);

    startupDispatcher.callSystems(world, cmds);

    while (!world.read<ShouldQuit>().get().value)
    {
        mainDispatcher.callSystems(world, cmds);
    }
}
