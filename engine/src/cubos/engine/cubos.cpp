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

Cubos& Cubos::putStageAfter(const std::string& stage, const std::string& referenceStage)
{
    if (isStartupStage)
    {
        startupDispatcher.putStageAfter(stage, referenceStage);
    }
    else
    {
        mainDispatcher.putStageAfter(stage, referenceStage);
    }
    return *this;
}

Cubos& Cubos::putStageBefore(const std::string& stage, const std::string& referenceStage)
{
    if (isStartupStage)
    {
        startupDispatcher.putStageBefore(stage, referenceStage);
    }
    else
    {
        mainDispatcher.putStageBefore(stage, referenceStage);
    }
    return *this;
}

Cubos::Cubos()
{
    core::initializeLogger();

    addResource<ShouldQuit>(true);
    addResource<cubos::core::Settings>();
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

    cubos::core::ecs::Commands cmds(world);

    startupDispatcher.callSystems(world, cmds);

    while (!world.read<ShouldQuit>().get().value)
    {
        mainDispatcher.callSystems(world, cmds);
    }
}
