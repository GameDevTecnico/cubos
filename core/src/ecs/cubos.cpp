#include <utility>

#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/cubos.hpp>
#include <cubos/core/ecs/observer/observers.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>

using cubos::core::ecs::Arguments;
using cubos::core::ecs::Cubos;
using cubos::core::ecs::DeltaTime;
using cubos::core::ecs::ShouldQuit;
using cubos::core::ecs::TagBuilder;

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

TagBuilder& TagBuilder::tagged(const std::string& tag)
{
    mDispatcher.tagInheritTag(tag);
    return *this;
}

Cubos::Cubos()
    : Cubos(1, nullptr)
{
}

Cubos::Cubos(int argc, char** argv)
{
    std::vector<std::string> arguments(argv + 1, argv + argc);

    this->addResource<DeltaTime>();
    this->addResource<ShouldQuit>();
    this->addResource<Arguments>(Arguments{.value = arguments});
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

        auto& dt = mWorld.write<DeltaTime>().get();
        dt.value = std::chrono::duration<float>(currentTime - previousTime).count() * dt.multiplier;
        previousTime = currentTime;
    } while (!mWorld.read<ShouldQuit>().get().value);
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
    return TagBuilder{mWorld, mMainDispatcher, mMainTags};
}

TagBuilder Cubos::startupTag(const std::string& tag)
{
    mStartupDispatcher.addTag(tag);
    return TagBuilder{mWorld, mStartupDispatcher, mStartupTags};
}

TagBuilder Cubos::noTag(const std::string& tag)
{
    mMainDispatcher.addNegativeTag(tag);
    return TagBuilder{mWorld, mMainDispatcher, mMainTags};
}

TagBuilder Cubos::noStartupTag(const std::string& tag)
{
    mStartupDispatcher.addNegativeTag(tag);
    return TagBuilder{mWorld, mStartupDispatcher, mStartupTags};
}

auto Cubos::system(std::string name) -> SystemBuilder
{
    return {mWorld, mMainDispatcher, std::move(name)};
}

auto Cubos::startupSystem(std::string name) -> SystemBuilder
{
    return {mWorld, mStartupDispatcher, std::move(name)};
}

auto Cubos::observer(std::string name) -> ObserverBuilder
{
    return {mWorld, std::move(name)};
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
    CUBOS_DEBUG("Added system {}", mName);
    mDispatcher.addSystem(std::move(mName), std::move(system));

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
}

Cubos::ObserverBuilder::ObserverBuilder(World& world, std::string name)
    : mWorld{world}
    , mName{std::move(name)}
{
}

auto Cubos::ObserverBuilder::onAdd(const reflection::Type& type, int target) && -> ObserverBuilder&&
{
    CUBOS_ASSERT(mWorld.types().contains(type), "No such component type {} was registered", type.name());
    auto dataTypeId = mWorld.types().id(type);
    CUBOS_ASSERT(mWorld.types().isComponent(dataTypeId), "Type {} isn't registered as a component", type.name());
    CUBOS_ASSERT(mColumnId == ColumnId::Invalid, "An observer can only have at most one hook");

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

    mOptions.back().observedTarget = target;
    mRemove = false;
    mColumnId = ColumnId::make(mWorld.types().id(type));
    return std::move(*this);
}

auto Cubos::ObserverBuilder::onRemove(const reflection::Type& type, int target) && -> ObserverBuilder&&
{
    CUBOS_ASSERT(mWorld.types().contains(type), "No such component type {} was registered", type.name());
    auto dataTypeId = mWorld.types().id(type);
    CUBOS_ASSERT(mWorld.types().isComponent(dataTypeId), "Type {} isn't registered as a component", type.name());
    CUBOS_ASSERT(mColumnId == ColumnId::Invalid, "An observer can only have at most one hook");

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

    mOptions.back().observedTarget = target;
    mRemove = true;
    mColumnId = ColumnId::make(mWorld.types().id(type));
    return std::move(*this);
}

auto Cubos::ObserverBuilder::entity(int target) && -> ObserverBuilder&&
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

auto Cubos::ObserverBuilder::with(const reflection::Type& type, int target) && -> ObserverBuilder&&
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

auto Cubos::ObserverBuilder::without(const reflection::Type& type, int target) && -> ObserverBuilder&&
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

auto Cubos::ObserverBuilder::related(const reflection::Type& type, int fromTarget, int toTarget) && -> ObserverBuilder&&
{
    return std::move(*this).related(type, Traversal::Random, fromTarget, toTarget);
}

auto Cubos::ObserverBuilder::related(const reflection::Type& type, Traversal traversal, int fromTarget,
                                     int toTarget) && -> ObserverBuilder&&
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

auto Cubos::ObserverBuilder::other() && -> ObserverBuilder&&
{
    mDefaultTarget = 0;
    mOptions.emplace_back();
    return std::move(*this);
}

void Cubos::ObserverBuilder::finish(System<void> system)
{
    CUBOS_ASSERT(mColumnId != ColumnId::Invalid, "You must set at least one hook for the observer");
    CUBOS_DEBUG("Added observer {}", mName);

    if (mRemove)
    {
        mWorld.observers().hookOnRemove(mColumnId, std::move(system));
    }
    else
    {
        mWorld.observers().hookOnAdd(mColumnId, std::move(system));
    }
}
