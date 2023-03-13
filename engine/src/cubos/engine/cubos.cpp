#include <cubos/core/log.hpp>
#include <cubos/core/settings.hpp>
#include <cubos/core/ecs/commands.hpp>

#include <cubos/engine/cubos.hpp>

using namespace cubos::engine;

ShouldQuit::ShouldQuit(bool value) : value(value)
{
}

Arguments::Arguments(const std::vector<std::string>& value) : value(value)
{
}

TagBuilder::TagBuilder(core::ecs::Dispatcher& dispatcher, std::vector<std::string>& tags)
    : dispatcher(dispatcher), tags(tags)
{
}

TagBuilder& TagBuilder::beforeTag(const std::string& tag)
{
    tags.push_back(tag);
    dispatcher.tagSetBeforeTag(tag);
    return *this;
}

TagBuilder& TagBuilder::afterTag(const std::string& tag)
{
    tags.push_back(tag);
    dispatcher.tagSetAfterTag(tag);
    return *this;
}

SystemBuilder::SystemBuilder(core::ecs::Dispatcher& dispatcher, std::vector<std::string>& tags)
    : dispatcher(dispatcher), tags(tags)
{
}

SystemBuilder& SystemBuilder::tagged(const std::string& tag)
{
    if (std::find(tags.begin(), tags.end(), tag) != tags.end())
    {
        CUBOS_WARN("Tag '{}' was defined on opposite system type (normal/startup), possible tag/startupTag mismatch? ",
                   tag);
    }
    dispatcher.systemAddTag(tag);
    return *this;
}

SystemBuilder& SystemBuilder::beforeTag(const std::string& tag)
{
    if (std::find(tags.begin(), tags.end(), tag) != tags.end())
    {
        CUBOS_WARN("Tag '{}' was defined on opposite system type (normal/startup), possible tag/startupTag mismatch? ",
                   tag);
    }
    dispatcher.systemSetBeforeTag(tag);
    return *this;
}

SystemBuilder& SystemBuilder::afterTag(const std::string& tag)
{
    if (std::find(tags.begin(), tags.end(), tag) != tags.end())
    {
        CUBOS_WARN("Tag '{}' was defined on opposite system type (normal/startup), possible tag/startupTag mismatch? ",
                   tag);
    }
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

TagBuilder Cubos::tag(const std::string& tag)
{
    mainDispatcher.addTag(tag);
    TagBuilder builder(mainDispatcher, mainTags);

    return builder;
}

TagBuilder Cubos::startupTag(const std::string& tag)
{
    startupDispatcher.addTag(tag);
    TagBuilder builder(startupDispatcher, startupTags);

    return builder;
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
    mainTags.clear();
    startupTags.clear();

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
