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
    isStartup = false;
    isSystem = false;

    return *this;
}

Cubos& Cubos::startupTag(const std::string& tag)
{
    startupDispatcher.addTag(tag);
    isStartup = true;
    isSystem = false;

    return *this;
}

Cubos& Cubos::tagged(const std::string& tag)
{
    if(isSystem)
    {
        if(isStartup)
        {
            startupDispatcher.systemSetTag(tag);
        }
        else
        {
            mainDispatcher.systemSetTag(tag);
        }
    } else
    {
        if(isStartup)
        {
            startupDispatcher.tagInheritTag(tag);
        }
        else
        {
            mainDispatcher.tagInheritTag(tag);
        }
    }

    return *this;
}

Cubos& Cubos::afterTag(const std::string& tag)
{
    if(isSystem)
    {
        if(isStartup)
        {
            startupDispatcher.systemSetAfterTag(tag);
        }
        else
        {
            mainDispatcher.systemSetAfterTag(tag);
        }
    } else
    {
        if(isStartup)
        {
            startupDispatcher.tagSetAfterTag(tag);
        }
        else
        {
            mainDispatcher.tagSetAfterTag(tag);
        }
    }
    return *this;
}

Cubos& Cubos::beforeTag(const std::string& tag)
{
    if(isSystem)
    {
        if(isStartup)
        {
            startupDispatcher.systemSetBeforeTag(tag);
        }
        else
        {
            mainDispatcher.systemSetBeforeTag(tag);
        }
    } else
    {
        if(isStartup)
        {
            startupDispatcher.tagSetBeforeTag(tag);
        }
        else
        {
            mainDispatcher.tagSetBeforeTag(tag);
        }
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

Cubos::Cubos(int argc, char** argv)
{
    std::vector<std::string> arguments(argv + 1, argv + argc);
    addResource<Arguments>(arguments);

    addResource<ShouldQuit>(true);
    addResource<cubos::core::Settings>();
}

void Cubos::run()
{
    plugins.clear();

    // Compile execution chain
    startupDispatcher.compileChain();
    mainDispatcher.compileChain();

    cubos::core::ecs::CommandBuffer cmds(world);

    startupDispatcher.callSystems(world, cmds);

    while (!world.read<ShouldQuit>().get().value)
    {
        mainDispatcher.callSystems(world, cmds);
    }
}
