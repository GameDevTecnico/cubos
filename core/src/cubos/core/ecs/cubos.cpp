#include <utility>

#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/cubos.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>

using cubos::core::ecs::Arguments;
using cubos::core::ecs::Cubos;
using cubos::core::ecs::DeltaTime;
using cubos::core::ecs::ShouldQuit;
using cubos::core::ecs::SystemBuilder;
using cubos::core::ecs::TagBuilder;

DeltaTime::DeltaTime(float value)
    : value(value)
{
}

ShouldQuit::ShouldQuit(bool value)
    : value(value)
{
}

Arguments::Arguments(std::vector<std::string> value)
    : value(std::move(value))
{
}

TagBuilder::TagBuilder(core::ecs::Dispatcher& dispatcher, std::vector<std::string>& tags)
    : mDispatcher(dispatcher)
    , mTags(tags)
{
}

TagBuilder& TagBuilder::before(const std::string& tag)
{
    mTags.push_back(tag);
    mDispatcher.tagSetBeforeTag(tag);
    return *this;
}

TagBuilder& TagBuilder::after(const std::string& tag)
{
    mTags.push_back(tag);
    mDispatcher.tagSetAfterTag(tag);
    return *this;
}

SystemBuilder::SystemBuilder(core::ecs::Dispatcher& dispatcher, std::vector<std::string>& tags)
    : mDispatcher(dispatcher)
    , mTags(tags)
{
}

Cubos& Cubos::addPlugin(void (*func)(Cubos&))
{
    if (!mPlugins.contains(func))
    {
        func(*this);
        mPlugins.insert(func);
    }
    else
    {
        CUBOS_TRACE("Plugin was already registered!");
    }
    return *this;
}

TagBuilder Cubos::tag(const std::string& tag)
{
    mMainDispatcher.addTag(tag);
    TagBuilder builder(mMainDispatcher, mMainTags);

    return builder;
}

TagBuilder Cubos::startupTag(const std::string& tag)
{
    mStartupDispatcher.addTag(tag);
    TagBuilder builder(mStartupDispatcher, mStartupTags);

    return builder;
}

auto Cubos::system(std::string name) -> SystemBuilder
{
    return {mMainDispatcher, std::move(name)};
}

auto Cubos::startupSystem(std::string name) -> SystemBuilder
{
    return {mStartupDispatcher, std::move(name)};
}

Cubos::SystemBuilder::SystemBuilder(Dispatcher& dispatcher, std::string name)
    : mDispatcher{dispatcher}
    , mName{std::move(name)}
{
}

auto Cubos::SystemBuilder::tagged(const std::string& tag) && -> SystemBuilder&&
{
    mTagged.insert(tag);
    return std::move(*this);
}

auto Cubos::SystemBuilder::before(const std::string& tag) && -> SystemBuilder&&
{
    mBefore.insert(tag);
    return std::move(*this);
}

auto Cubos::SystemBuilder::after(const std::string& tag) && -> SystemBuilder&&
{
    mAfter.insert(tag);
    return std::move(*this);
}

void Cubos::SystemBuilder::finish(std::shared_ptr<AnySystemWrapper<void>> system)
{
    mDispatcher.addSystem(std::move(system));

    if (mCondition != nullptr)
    {
        mDispatcher.systemAddCondition(mCondition);
    }

    for (const auto& tag : mTagged)
    {
        mDispatcher.systemAddTag(tag);
    }

    for (const auto& tag : mBefore)
    {
        mDispatcher.systemSetBeforeTag(tag);
    }

    for (const auto& tag : mAfter)
    {
        mDispatcher.systemSetAfterTag(tag);
    }

    CUBOS_DEBUG("Added system {}", mName);
}

Cubos::Cubos()
    : Cubos(1, nullptr)
{
}

Cubos::Cubos(int argc, char** argv)
{
    std::vector<std::string> arguments(argv + 1, argv + argc);

    this->addResource<DeltaTime>(0.0F);
    this->addResource<ShouldQuit>(true);
    this->addResource<Arguments>(arguments);
}

void Cubos::run()
{
    mPlugins.clear();
    mMainTags.clear();
    mStartupTags.clear();

    // Compile execution chain
    mStartupDispatcher.compileChain();
    mMainDispatcher.compileChain();

    cubos::core::ecs::CommandBuffer cmds(mWorld);

    mStartupDispatcher.callSystems(mWorld, cmds);

    auto currentTime = std::chrono::steady_clock::now();
    auto previousTime = std::chrono::steady_clock::now();
    do
    {
        mMainDispatcher.callSystems(mWorld, cmds);
        currentTime = std::chrono::steady_clock::now();
        mWorld.write<DeltaTime>().get().value = std::chrono::duration<float>(currentTime - previousTime).count();
        previousTime = currentTime;
    } while (!mWorld.read<ShouldQuit>().get().value);
}
