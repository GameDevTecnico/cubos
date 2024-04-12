#include <utility>

#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/cubos.hpp>
#include <cubos/core/ecs/name.hpp>
#include <cubos/core/ecs/observer/observers.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/vector.hpp>

using cubos::core::ecs::Arguments;
using cubos::core::ecs::Cubos;
using cubos::core::ecs::DeltaTime;
using cubos::core::ecs::Name;
using cubos::core::ecs::ShouldQuit;

CUBOS_REFLECT_IMPL(DeltaTime)
{
    return TypeBuilder<DeltaTime>("cubos::core::ecs::DeltaTime")
        .withField("unscaledValue", &DeltaTime::unscaledValue)
        .withField("scale", &DeltaTime::scale)
        .build();
}

CUBOS_REFLECT_IMPL(ShouldQuit)
{
    return TypeBuilder<ShouldQuit>("cubos::core::ecs::ShouldQuit").withField("value", &ShouldQuit::value).build();
}

CUBOS_REFLECT_IMPL(Arguments)
{
    return TypeBuilder<Arguments>("cubos::core::ecs::Arguments").withField("value", &Arguments::value).build();
}

Cubos::Cubos()
    : Cubos(1, nullptr)
{
}

Cubos::Cubos(int argc, char** argv)
{
    std::vector<std::string> arguments(argv + 1, argv + argc);

    this->resource<DeltaTime>();
    this->resource<ShouldQuit>();
    this->resource<Arguments>(Arguments{.value = arguments});
}

Cubos& Cubos::plugin(Plugin plugin)
{
    CUBOS_ASSERT(!mInstalledPlugins.contains(plugin), "Plugin has already been added by another plugin");

    mInstalledPlugins.at(mPluginStack.back()).subPlugins.emplace(plugin);
    mInstalledPlugins.insert({plugin, PluginInfo{}});

    mPluginStack.push_back(plugin);
    plugin(*this);
    mPluginStack.pop_back();

    return *this;
}

Cubos& Cubos::depends(Plugin plugin)
{
    CUBOS_ASSERT(mInstalledPlugins.contains(plugin),
                 "Plugin dependency wasn't fulfilled. Did you forget to add a plugin?");

    mInstalledPlugins.at(plugin).dependentCount += 1;
    mInstalledPlugins.at(mPluginStack.back()).dependencies.emplace(plugin);

    return *this;
}

Cubos& Cubos::uninitResource(const reflection::Type& type)
{
    CUBOS_ASSERT(!mTypeToPlugin.contains(type), "Resource {} was already registered", type.name());
    mTypeToPlugin.insert(type, mPluginStack.back());
    mWorld.registerResource(type);
    return *this;
}

Cubos& Cubos::resource(memory::AnyValue value)
{
    this->uninitResource(value.type());
    mWorld.insertResource(std::move(value));
    return *this;
}

Cubos& Cubos::component(const reflection::Type& type)
{
    CUBOS_ASSERT(!mTypeToPlugin.contains(type), "Component {} was already registered", type.name());
    mTypeToPlugin.insert(type, mPluginStack.back());
    mWorld.registerComponent(type);
    return *this;
}

Cubos& Cubos::relation(const reflection::Type& type)
{
    CUBOS_ASSERT(!mTypeToPlugin.contains(type), "Relation {} was already registered", type.name());
    mTypeToPlugin.insert(type, mPluginStack.back());
    mWorld.registerRelation(type);
    return *this;
}

Cubos::TagBuilder Cubos::tag(const Tag& tag)
{
    if (!this->isRegistered(tag))
    {
        CUBOS_ASSERT(!mTags.contains(tag.id()),
                     "Tag {} was already registered by another plugin - if you depend on it, define the dependency",
                     tag.name());
        mTags.emplace(tag.id(), TagInfo{
                                    .isStartup = false,
                                    .id = mMainPlanner.add(),
                                    .plugin = mPluginStack.back(),
                                });
    }

    return {*this, false, mTags.at(tag.id()).id};
}

Cubos::TagBuilder Cubos::startupTag(const Tag& tag)
{
    if (!this->isRegistered(tag))
    {
        CUBOS_ASSERT(!mTags.contains(tag.id()),
                     "Tag {} was already registered by another plugin - if you depend on it, define the dependency",
                     tag.name());
        mTags.emplace(tag.id(), TagInfo{
                                    .isStartup = true,
                                    .id = mStartupPlanner.add(),
                                    .plugin = mPluginStack.back(),
                                });
    }

    return {*this, true, mTags.at(tag.id()).id};
}

auto Cubos::system(std::string name) -> SystemBuilder
{
    return {*this, false, std::move(name)};
}

auto Cubos::startupSystem(std::string name) -> SystemBuilder
{
    return {*this, true, std::move(name)};
}

auto Cubos::observer(std::string name) -> ObserverBuilder
{
    return {*this, std::move(name)};
}

void Cubos::run()
{
    // Generate schedules.
    auto startupSchedule = mStartupPlanner.build();
    CUBOS_ASSERT(startupSchedule.contains(), "Couldn't build initial startup schedule");
    auto mainSchedule = mMainPlanner.build();
    CUBOS_ASSERT(mainSchedule.contains(), "Couldn't build initial main schedule");

    CommandBuffer cmdBuffer(mWorld);
    SystemContext ctx{.cmdBuffer = cmdBuffer};

    startupSchedule->run(mSystemRegistry, ctx);

    auto currentTime = std::chrono::steady_clock::now();
    auto previousTime = std::chrono::steady_clock::now();
    do
    {
        mainSchedule->run(mSystemRegistry, ctx);
        currentTime = std::chrono::steady_clock::now();

        auto& dt = mWorld.resource<DeltaTime>();
        dt.unscaledValue = std::chrono::duration<float>(currentTime - previousTime).count();
        previousTime = currentTime;
    } while (!mWorld.resource<ShouldQuit>().value);
}

bool Cubos::isRegistered(const reflection::Type& type) const
{
    return (mTypeToPlugin.contains(type) && this->isKnownPlugin(mTypeToPlugin.at(type), mPluginStack.back())) ||
           type.is<Name>() || type.is<Arguments>() || type.is<DeltaTime>() || type.is<ShouldQuit>();
}

bool Cubos::isRegistered(const Tag& tag) const
{
    return mTags.contains(tag.id()) && this->isKnownPlugin(mTags.at(tag.id()).plugin, mPluginStack.back());
}

bool Cubos::isSubPlugin(Plugin subPlugin, Plugin basePlugin) const
{
    for (auto p : mInstalledPlugins.at(basePlugin).subPlugins)
    {
        if (p == subPlugin || this->isSubPlugin(subPlugin, p))
        {
            return true;
        }
    }

    return false;
}

bool Cubos::isKnownPlugin(Plugin plugin, Plugin basePlugin) const
{
    if (plugin == basePlugin || this->isSubPlugin(plugin, basePlugin))
    {
        return true;
    }

    for (auto dependency : mInstalledPlugins.at(basePlugin).dependencies)
    {
        if (plugin == dependency || this->isSubPlugin(plugin, dependency))
        {
            return true;
        }
    }

    return false;
}

Cubos::TagBuilder::TagBuilder(Cubos& cubos, bool isStartup, Planner::TagId tagId)
    : mCubos{cubos}
    , mPlanner{isStartup ? cubos.mStartupPlanner : cubos.mMainPlanner}
    , mIsStartup{isStartup}
    , mTagId{tagId}
{
}

Cubos::TagBuilder& Cubos::TagBuilder::before(const Tag& tag)
{
    CUBOS_ASSERT(mCubos.isRegistered(tag), "Tag {} wasn't registered by the plugin or its dependencies", tag.name());
    CUBOS_ASSERT(mCubos.mTags.at(tag.id()).isStartup == mIsStartup,
                 "Cannot make tag run before {}, as one is a startup tag and the other isn't", tag.name());
    mPlanner.order(mTagId, mCubos.mTags.at(tag.id()).id);
    return *this;
}

Cubos::TagBuilder& Cubos::TagBuilder::after(const Tag& tag)
{
    CUBOS_ASSERT(mCubos.isRegistered(tag), "Tag {} wasn't registered by the plugin or its dependencies", tag.name());
    CUBOS_ASSERT(mCubos.mTags.at(tag.id()).isStartup == mIsStartup,
                 "Cannot make tag run after {}, as one is a startup tag and the other isn't", tag.name());
    mPlanner.order(mCubos.mTags.at(tag.id()).id, mTagId);
    return *this;
}

Cubos::TagBuilder& Cubos::TagBuilder::tagged(const Tag& tag)
{
    CUBOS_ASSERT(mCubos.isRegistered(tag), "Tag {} wasn't registered by the plugin or its dependencies", tag.name());
    CUBOS_ASSERT(mCubos.mTags.at(tag.id()).isStartup == mIsStartup,
                 "Cannot tag with {}, as one is a startup tag and the other isn't", tag.name());
    CUBOS_ASSERT(mPlanner.tag(mTagId, mCubos.mTags.at(tag.id()).id));
    return *this;
}

Cubos::TagBuilder& Cubos::TagBuilder::addTo(const Tag& tag)
{
    CUBOS_ASSERT(mCubos.isRegistered(tag), "Tag {} wasn't registered by the plugin or its dependencies", tag.name());
    CUBOS_ASSERT(mCubos.mTags.at(tag.id()).isStartup == mIsStartup,
                 "Cannot add tag {} to tag, as one is a startup tag and the other isn't", tag.name());
    CUBOS_ASSERT(mPlanner.tag(mCubos.mTags.at(tag.id()).id, mTagId));
    return *this;
}

Cubos::SystemBuilder::SystemBuilder(Cubos& cubos, bool isStartup, std::string name)
    : mCubos{cubos}
    , mIsStartup{isStartup}
    , mName{std::move(name)}
{
}

auto Cubos::SystemBuilder::tagged(const Tag& tag) && -> SystemBuilder&&
{
    CUBOS_ASSERT(mCubos.isRegistered(tag), "Tag {} wasn't registered by the plugin or its dependencies", tag.name());
    CUBOS_ASSERT(mIsStartup == mCubos.mTags.at(tag.id()).isStartup,
                 "Cannot tag {} with {}, as one is startup and the other isn't", mName, tag.name());
    mTagged.insert(tag.id());
    return std::move(*this);
}

auto Cubos::SystemBuilder::before(const Tag& tag) && -> SystemBuilder&&
{
    CUBOS_ASSERT(mCubos.isRegistered(tag), "Tag {} wasn't registered by the plugin or its dependencies", tag.name());
    CUBOS_ASSERT(mIsStartup == mCubos.mTags.at(tag.id()).isStartup,
                 "Cannot make {} run before {}, as one is startup and the other isn't", mName, tag.name());
    mBefore.insert(tag.id());
    return std::move(*this);
}

auto Cubos::SystemBuilder::after(const Tag& tag) && -> SystemBuilder&&
{
    CUBOS_ASSERT(mCubos.isRegistered(tag), "Tag {} wasn't registered by the plugin or its dependencies", tag.name());
    CUBOS_ASSERT(mIsStartup == mCubos.mTags.at(tag.id()).isStartup,
                 "Cannot make {} run after {}, as one is startup and the other isn't", mName, tag.name());
    mAfter.insert(tag.id());
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
    CUBOS_ASSERT(mCubos.isRegistered(type), "No such component type {} was registered", type.name());
    auto dataTypeId = mCubos.mWorld.types().id(type);
    CUBOS_ASSERT(mCubos.mWorld.types().isComponent(dataTypeId), "Type {} isn't registered as a component", type.name());

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
    CUBOS_ASSERT(mCubos.isRegistered(type), "No such component type {} was registered", type.name());
    auto dataTypeId = mCubos.mWorld.types().id(type);
    CUBOS_ASSERT(mCubos.mWorld.types().isComponent(dataTypeId), "Type {} isn't registered as a component", type.name());

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
    CUBOS_ASSERT(mCubos.isRegistered(type), "No such relation type {} was registered", type.name());
    auto dataTypeId = mCubos.mWorld.types().id(type);
    CUBOS_ASSERT(mCubos.mWorld.types().isRelation(dataTypeId), "Type {} isn't registered as a relation", type.name());

    CUBOS_ASSERT_IMP(traversal != Traversal::Random, mCubos.mWorld.types().isTreeRelation(dataTypeId),
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
    if (mCondition.contains())
    {
        for (const auto& dataTypeId : mCondition.value().access().dataTypes)
        {
            const auto& type = mCubos.mWorld.types().type(dataTypeId);
            CUBOS_ASSERT(mCubos.isRegistered(type), "Type {} isn't registered by the plugin or its dependencies",
                         type.name());
        }
    }

    for (const auto& dataTypeId : system.access().dataTypes)
    {
        const auto& type = mCubos.mWorld.types().type(dataTypeId);
        CUBOS_ASSERT(mCubos.isRegistered(type), "Type {} isn't registered by the plugin or its dependencies",
                     type.name());
    }

    CUBOS_DEBUG("Added system {}", mName);
    auto& planner = mIsStartup ? mCubos.mStartupPlanner : mCubos.mMainPlanner;
    auto systemId = mCubos.mSystemRegistry.add(mName, std::move(system));
    auto leafId = planner.add(mName, systemId);

    if (mCondition)
    {
        auto conditionId = mCubos.mSystemRegistry.add(mName + "#condition", std::move(mCondition.value()));
        planner.onlyIf(leafId, conditionId);
    }

    for (const auto& tag : mTagged)
    {
        CUBOS_ASSERT(planner.tag(leafId, mCubos.mTags.at(tag).id));
    }

    for (const auto& tag : mBefore)
    {
        planner.order(leafId, mCubos.mTags.at(tag).id);
    }

    for (const auto& tag : mAfter)
    {
        planner.order(mCubos.mTags.at(tag).id, leafId);
    }
}

Cubos::ObserverBuilder::ObserverBuilder(Cubos& cubos, std::string name)
    : mCubos{cubos}
    , mName{std::move(name)}
{
}

auto Cubos::ObserverBuilder::onAdd(const reflection::Type& type, int target) && -> ObserverBuilder&&
{
    CUBOS_ASSERT(mCubos.isRegistered(type), "No such component type {} was registered", type.name());
    auto dataTypeId = mCubos.mWorld.types().id(type);
    CUBOS_ASSERT(mCubos.mWorld.types().isComponent(dataTypeId), "Type {} isn't registered as a component", type.name());
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
    mColumnId = ColumnId::make(mCubos.mWorld.types().id(type));
    return std::move(*this);
}

auto Cubos::ObserverBuilder::onRemove(const reflection::Type& type, int target) && -> ObserverBuilder&&
{
    CUBOS_ASSERT(mCubos.isRegistered(type), "No such component type {} was registered", type.name());
    auto dataTypeId = mCubos.mWorld.types().id(type);
    CUBOS_ASSERT(mCubos.mWorld.types().isComponent(dataTypeId), "Type {} isn't registered as a component", type.name());
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
    mColumnId = ColumnId::make(mCubos.mWorld.types().id(type));
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
    CUBOS_ASSERT(mCubos.isRegistered(type), "No such component type {} was registered", type.name());
    auto dataTypeId = mCubos.mWorld.types().id(type);
    CUBOS_ASSERT(mCubos.mWorld.types().isComponent(dataTypeId), "Type {} isn't registered as a component", type.name());

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
    CUBOS_ASSERT(mCubos.isRegistered(type), "No such component type {} was registered", type.name());
    auto dataTypeId = mCubos.mWorld.types().id(type);
    CUBOS_ASSERT(mCubos.mWorld.types().isComponent(dataTypeId), "Type {} isn't registered as a component", type.name());

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
    CUBOS_ASSERT(mCubos.isRegistered(type), "No such relation type {} was registered", type.name());
    auto dataTypeId = mCubos.mWorld.types().id(type);
    CUBOS_ASSERT(mCubos.mWorld.types().isRelation(dataTypeId), "Type {} isn't registered as a relation", type.name());

    CUBOS_ASSERT_IMP(traversal != Traversal::Random, mCubos.mWorld.types().isTreeRelation(dataTypeId),
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
    for (const auto& dataTypeId : system.access().dataTypes)
    {
        const auto& type = mCubos.mWorld.types().type(dataTypeId);
        CUBOS_ASSERT(mCubos.isRegistered(type), "Type {} isn't registered by the plugin or its dependencies",
                     type.name());
    }

    CUBOS_ASSERT(mColumnId != ColumnId::Invalid, "You must set at least one hook for the observer");
    CUBOS_DEBUG("Added observer {}", mName);

    if (mRemove)
    {
        mCubos.mWorld.observers().hookOnRemove(mColumnId, std::move(system));
    }
    else
    {
        mCubos.mWorld.observers().hookOnAdd(mColumnId, std::move(system));
    }
}
