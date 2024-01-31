#include <utility>

#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/cubos.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>

using cubos::core::ecs::Arguments;
using cubos::core::ecs::Cubos;
using cubos::core::ecs::DeltaTime;
using cubos::core::ecs::ShouldQuit;
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

TagBuilder::TagBuilder(World& world, core::ecs::Dispatcher& dispatcher, std::vector<std::string>& tags)
    : mWorld(world)
    , mDispatcher(dispatcher)
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
    TagBuilder builder(mWorld, mMainDispatcher, mMainTags);

    return builder;
}

TagBuilder Cubos::startupTag(const std::string& tag)
{
    mStartupDispatcher.addTag(tag);
    TagBuilder builder(mWorld, mStartupDispatcher, mStartupTags);

    return builder;
}

auto Cubos::system(std::string name) -> SystemBuilder
{
    return {mWorld, mMainDispatcher, std::move(name)};
}

auto Cubos::startupSystem(std::string name) -> SystemBuilder
{
    return {mWorld, mStartupDispatcher, std::move(name)};
}

Cubos::SystemBuilder::SystemBuilder(World& world, Dispatcher& dispatcher, std::string name)
    : mWorld{world}
    , mDispatcher{dispatcher}
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

auto Cubos::SystemBuilder::entity(int target) && -> SystemBuilder&&
{
    if (mOptions.empty())
    {
        mOptions.emplace_back();
    }

    if (target == -1)
    {
        target = mDefaultTarget;
    }
    else
    {
        mDefaultTarget = target;
    }

    mOptions.back().queryTerms.emplace_back(QueryTerm::makeEntity(target));
    return std::move(*this);
}

auto Cubos::SystemBuilder::with(const reflection::Type& type, int target) && -> SystemBuilder&&
{
    CUBOS_ASSERT(mWorld.types().contains(type), "No such component type {} was registered", type.name());
    auto dataTypeId = mWorld.types().id(type);
    CUBOS_ASSERT(mWorld.types().isComponent(dataTypeId), "Type {} isn't registered as a component", type.name());

    if (mOptions.empty())
    {
        mOptions.emplace_back();
    }

    if (target == -1)
    {
        target = mDefaultTarget;
    }
    else
    {
        mDefaultTarget = target;
    }

    mOptions.back().queryTerms.emplace_back(QueryTerm::makeWithComponent(dataTypeId, target));
    return std::move(*this);
}

auto Cubos::SystemBuilder::without(const reflection::Type& type, int target) && -> SystemBuilder&&
{
    CUBOS_ASSERT(mWorld.types().contains(type), "No such component type {} was registered", type.name());
    auto dataTypeId = mWorld.types().id(type);
    CUBOS_ASSERT(mWorld.types().isComponent(dataTypeId), "Type {} isn't registered as a component", type.name());

    if (mOptions.empty())
    {
        mOptions.emplace_back();
    }

    if (target == -1)
    {
        target = mDefaultTarget;
    }
    else
    {
        mDefaultTarget = target;
    }

    mOptions.back().queryTerms.emplace_back(QueryTerm::makeWithoutComponent(dataTypeId, target));
    return std::move(*this);
}

auto Cubos::SystemBuilder::related(const reflection::Type& type, int fromTarget, int toTarget) && -> SystemBuilder&&
{
    return std::move(*this).related(type, Traversal::Random, fromTarget, toTarget);
}

auto Cubos::SystemBuilder::related(const reflection::Type& type, Traversal traversal, int fromTarget,
                                   int toTarget) && -> SystemBuilder&&
{
    CUBOS_ASSERT(mWorld.types().contains(type), "No such relation type {} was registered", type.name());
    auto dataTypeId = mWorld.types().id(type);
    CUBOS_ASSERT(mWorld.types().isRelation(dataTypeId), "Type {} isn't registered as a relation", type.name());

    CUBOS_ASSERT_IMP(traversal != Traversal::Random, mWorld.types().isTreeRelation(dataTypeId),
                     "Directed traversal can only be used for tree relations, and {} isn't registered as one",
                     type.name());

    if (mOptions.empty())
    {
        mOptions.emplace_back();
    }

    if (fromTarget == -1)
    {
        fromTarget = mDefaultTarget;
    }

    if (toTarget == -1)
    {
        toTarget = fromTarget + 1;
    }

    mDefaultTarget = toTarget;
    mOptions.back().queryTerms.emplace_back(QueryTerm::makeRelation(dataTypeId, fromTarget, toTarget, traversal));
    return std::move(*this);
}

auto Cubos::SystemBuilder::other() && -> SystemBuilder&&
{
    mDefaultTarget = 0;
    mOptions.emplace_back();
    return std::move(*this);
}

void Cubos::SystemBuilder::finish(System<void> system)
{
    mDispatcher.addSystem(std::move(system));

    if (mCondition)
    {
        mDispatcher.systemAddCondition(std::move(mCondition.value()));
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
    : mWorld{}
    , mStartupDispatcher{mWorld}
    , mMainDispatcher{mWorld}
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

    mStartupDispatcher.callSystems(cmds);

    auto currentTime = std::chrono::steady_clock::now();
    auto previousTime = std::chrono::steady_clock::now();
    do
    {
        mMainDispatcher.callSystems(cmds);
        currentTime = std::chrono::steady_clock::now();
        mWorld.write<DeltaTime>().get().value = std::chrono::duration<float>(currentTime - previousTime).count();
        previousTime = currentTime;
    } while (!mWorld.read<ShouldQuit>().get().value);
}
