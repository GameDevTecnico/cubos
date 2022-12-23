#include <cubos/core/log.hpp>
#include <cubos/core/settings.hpp>
#include <cubos/core/ecs/commands.hpp>

#include <cubos/engine/cubos.hpp>

using namespace cubos::engine;

TagBuilder::TagBuilder(core::ecs::Dispatcher& dispatcher) : dispatcher(dispatcher)
{
}

TagBuilder& TagBuilder::beforeTag(const std::string& tag)
{
    dispatcher.tagSetBeforeTag(tag);
    return *this;
}

TagBuilder& TagBuilder::afterTag(const std::string& tag)
{
    dispatcher.tagSetAfterTag(tag);
    return *this;
}

SystemBuilder::SystemBuilder(core::ecs::Dispatcher& dispatcher) : dispatcher(dispatcher)
{
}

SystemBuilder& SystemBuilder::tagged(const std::string& tag)
{
    dispatcher.systemSetTag(tag);
    return *this;
}

SystemBuilder& SystemBuilder::beforeTag(const std::string& tag)
{
    dispatcher.systemSetBeforeTag(tag);
    return *this;
}

SystemBuilder& SystemBuilder::afterTag(const std::string& tag)
{
    dispatcher.systemSetAfterTag(tag);
    return *this;
}

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

TagBuilder& Cubos::tag(const std::string& tag)
{
    mainDispatcher.addTag(tag);
    TagBuilder* builder = new TagBuilder(mainDispatcher);

    return *builder;
}

TagBuilder& Cubos::startupTag(const std::string& tag)
{
    startupDispatcher.addTag(tag);
    TagBuilder* builder = new TagBuilder(startupDispatcher);

    return *builder;
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
