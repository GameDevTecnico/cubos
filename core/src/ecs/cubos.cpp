#include <utility>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/cubos.hpp>
#include <cubos/core/ecs/name.hpp>
#include <cubos/core/ecs/observer/observers.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/memory/opt.hpp>
#include <cubos/core/net/tcp_listener.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/tel/logging.hpp>

#include "debugger.hpp"

using cubos::core::ecs::Arguments;
using cubos::core::ecs::Cubos;
using cubos::core::ecs::DeltaTime;
using cubos::core::ecs::Name;
using cubos::core::ecs::ShouldQuit;
using cubos::core::memory::Opt;
using cubos::core::net::Address;
using cubos::core::net::TcpListener;
using cubos::core::net::TcpStream;

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

struct Cubos::State
{
    CommandBuffer cmdBuffer;
    PluginQueue pluginQueue;
    Opt<Schedule> startupSchedule;
    Opt<Schedule> mainSchedule;
    std::chrono::steady_clock::time_point lastUpdateTime;
};

Cubos::~Cubos()
{
    delete mState;
    delete mWorld;
}

Cubos::Cubos()
    : Cubos(1, nullptr)
{
}

Cubos::Cubos(int argc, char** argv)
    : mWorld{new World{}}
{
    std::vector<std::string> arguments{};

    // Filter command line arguments before storing them as a resource.
    for (int i = 1; i < argc; ++i)
    {
        std::string arg{argv[i]};

        if (arg == "--debug")
        {
            ++i;

            if (i >= argc)
            {
                CUBOS_ERROR("Missing port number argument for --debug option");
                continue;
            }

            try
            {
                mDebug.replace({.port = static_cast<uint16_t>(std::stoi(argv[i]))});
            }
            catch (const std::invalid_argument&)
            {
                CUBOS_ERROR("Invalid port number argument for --debug option");
            }
        }
        else
        {
            arguments.emplace_back(argv[i]);
        }
    }

    this->resource<DeltaTime>();
    this->resource<ShouldQuit>();
    this->resource<Arguments>(Arguments{.value = arguments});
}

Cubos::Cubos(Cubos&& other) noexcept
    : mWorld{other.mWorld}
    , mSystemRegistry{std::move(other.mSystemRegistry)}
    , mState{other.mState}
    , mStartupPlanner{std::move(other.mStartupPlanner)}
    , mMainPlanner{std::move(other.mMainPlanner)}
    , mPluginStack{std::move(other.mPluginStack)}
    , mInstalledPlugins{std::move(other.mInstalledPlugins)}
    , mInjectedPlugins{std::move(other.mInjectedPlugins)}
    , mTypeToPlugin{std::move(other.mTypeToPlugin)}
    , mTags{std::move(other.mTags)}
{
    other.mWorld = nullptr;
}

Cubos& Cubos::plugin(Plugin plugin)
{
    if (mInjectedPlugins.contains(plugin))
    {
        plugin = mInjectedPlugins.at(plugin);
    }

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
    if (mInjectedPlugins.contains(plugin))
    {
        plugin = mInjectedPlugins.at(plugin);
    }

    CUBOS_ASSERT(mInstalledPlugins.contains(plugin),
                 "Plugin dependency wasn't fulfilled. Did you forget to add a plugin?");

    mInstalledPlugins.at(plugin).dependentCount += 1;
    mInstalledPlugins.at(mPluginStack.back()).dependencies.emplace(plugin);

    return *this;
}

Cubos& Cubos::inject(Plugin target, Plugin plugin)
{
    CUBOS_ASSERT(!mInstalledPlugins.contains(target), "Target plugin has already been added");
    CUBOS_ASSERT(!mInjectedPlugins.contains(target), "Target plugin has already been injected into");

    mInjectedPlugins.insert({target, plugin});

    return *this;
}

Cubos& Cubos::uninitResource(const reflection::Type& type)
{
    CUBOS_ASSERT(!mTypeToPlugin.contains(type), "Resource {} was already registered", type.name());
    mTypeToPlugin.insert(type, mPluginStack.back());
    mWorld->registerResource(type);
    return *this;
}

Cubos& Cubos::resource(memory::AnyValue value)
{
    this->uninitResource(value.type());
    mWorld->insertResource(std::move(value));
    return *this;
}

Cubos& Cubos::component(const reflection::Type& type)
{
    CUBOS_ASSERT(!mTypeToPlugin.contains(type), "Component {} was already registered", type.name());
    mTypeToPlugin.insert(type, mPluginStack.back());
    mWorld->registerComponent(type);
    return *this;
}

Cubos& Cubos::relation(const reflection::Type& type)
{
    CUBOS_ASSERT(!mTypeToPlugin.contains(type), "Relation {} was already registered", type.name());
    mTypeToPlugin.insert(type, mPluginStack.back());
    mWorld->registerRelation(type);
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
        mInstalledPlugins.at(mPluginStack.back()).tags.push_back(mTags.at(tag.id()).id);
    }

    return {*this, tag.name(), false, mTags.at(tag.id()).id};
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

    return {*this, tag.name(), true, mTags.at(tag.id()).id};
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

auto Cubos::typeServer() -> memory::Opt<reflection::TypeServer&>
{
    if (mDebug.contains())
    {
        return mDebug->typeServer;
    }

    return {};
}

void Cubos::reset()
{
    delete mState;
    mState = nullptr;

    mWorld->reset();
    mSystemRegistry.reset();
    mStartupPlanner.clear();
    mMainPlanner.clear();
    mInstalledPlugins = {{nullptr, {}}};
    mTypeToPlugin.clear();
    mTags.clear();

    this->resource<DeltaTime>();
    this->resource<ShouldQuit>();
}

void Cubos::start()
{
    CUBOS_ASSERT(mState == nullptr, "Cubos has already been started");
    mState = new State{
        .cmdBuffer = {*mWorld},
        .pluginQueue = {},
        .startupSchedule = {},
        .mainSchedule = {},
        .lastUpdateTime = {},
    };

    // Generate schedules.
    auto startupSchedule = mStartupPlanner.build();
    CUBOS_ASSERT(startupSchedule.contains(), "Couldn't build initial startup schedule");
    mState->startupSchedule.replace(startupSchedule.value());
    CUBOS_DEBUG("Built base startup schedule:\n{}", '\n' + startupSchedule->debug(mSystemRegistry) + '\n');
    mStartupPlanner.clear();

    auto mainSchedule = mMainPlanner.build();
    CUBOS_ASSERT(mainSchedule.contains(), "Couldn't build initial main schedule");
    mState->mainSchedule.replace(mainSchedule.value());
    CUBOS_DEBUG("Built base main schedule:\n{}", '\n' + mState->mainSchedule->debug(mSystemRegistry) + '\n');

    // Run startup systems.
    SystemContext ctx{.cmdBuffer = mState->cmdBuffer, .pluginQueue = &mState->pluginQueue};
    mState->startupSchedule->run(mSystemRegistry, ctx);

    mState->lastUpdateTime = std::chrono::steady_clock::now();
}

bool Cubos::update()
{
    CUBOS_ASSERT(mState != nullptr, "Cubos hasn't been started yet");

    SystemContext ctx{.cmdBuffer = mState->cmdBuffer, .pluginQueue = &mState->pluginQueue};

    // Remove and add plugins.
    for (auto plugin : mState->pluginQueue.toRemove)
    {
        this->uninstall(plugin);
    }
    for (auto plugin : mState->pluginQueue.toDestroy)
    {
        this->uninstall(plugin);
    }
    for (auto plugin : mState->pluginQueue.toAdd)
    {
        this->install(plugin);
    }

    // If any plugins were added, recompile the system execution chains, and rerun the startup systems.
    if (!mState->pluginQueue.toAdd.empty() || !mState->pluginQueue.toRemove.empty() || !mState->pluginQueue.toDestroy.empty())
    {
        // Clear the plugin queue.
        mState->pluginQueue.toRemove.clear();
        mState->pluginQueue.toDestroy.clear();
        mState->pluginQueue.toAdd.clear();

        // Build new schedules.
        auto newStartupSchedule = mStartupPlanner.build();
        CUBOS_ASSERT(newStartupSchedule.contains(), "Couldn't build updated startup schedule");
        mState->startupSchedule.replace(newStartupSchedule.value());
        CUBOS_DEBUG("Built new startup schedule:\n{}", '\n' + mState->startupSchedule->debug(mSystemRegistry) + '\n');
        mStartupPlanner.clear();

        auto newMainSchedule = mMainPlanner.build();
        CUBOS_ASSERT(newMainSchedule.contains(), "Couldn't build updated main schedule");
        mState->mainSchedule.replace(newMainSchedule.value());
        CUBOS_DEBUG("Built new main schedule:\n{}", '\n' + mState->mainSchedule->debug(mSystemRegistry) + '\n');

        // Run the newly installed startup systems.
        mState->startupSchedule->run(mSystemRegistry, ctx);
    }

    // Run main systems.
    mState->mainSchedule->run(mSystemRegistry, ctx);

    // Update delta time.
    auto currentTime = std::chrono::steady_clock::now();
    mWorld->resource<DeltaTime>().unscaledValue =
        std::chrono::duration<float>(currentTime - mState->lastUpdateTime).count();
    mState->lastUpdateTime = currentTime;

    return !this->shouldQuit();
}

void Cubos::run()
{
#ifdef __EMSCRIPTEN__
    // Emscripten requires looping to be done through a callback.
    auto* cubos = new Cubos(std::move(*this)); // Move the Cubos object to the heap so that it won't be destroyed.
    emscripten_set_main_loop_arg(
        [](void* cubos) {
            if (static_cast<Cubos*>(cubos)->update() == 0)
            {
                emscripten_cancel_main_loop();
                delete static_cast<Cubos*>(cubos);
            }
        },
        cubos, 0, 0);
    cubos->start();
#else
    // If debugging is disabled, simply run the update loop.
    if (!mDebug.contains())
    {
        this->start();
        while (this->update())
        {
        }

        return;
    }

    // Add all known types to the type server.
    for (const auto& [type, _] : mTypeToPlugin)
    {
        mDebug->typeServer.addType(*type);
    }

    // Start a TCP listener and wait for a connection.
    TcpListener listener;
    if (!listener.listen(Address::Any, mDebug->port))
    {
        CUBOS_CRITICAL("Debugger failed to start listening on port {}, shutting down", mDebug->port);
        return;
    }

    CUBOS_INFO("Debugging enabled on port {}", mDebug->port);

    while (true)
    {
        CUBOS_INFO("Debugger waiting for a connection");

        TcpStream stream;
        if (!listener.accept(stream))
        {
            CUBOS_CRITICAL("Debugger failed to accept connection, shutting down");
            break;
        }

        CUBOS_INFO("Debugger connected to a client");

        if (!debugger(stream, *this))
        {
            break;
        }

        CUBOS_INFO("Debugger disconnected from client");
    }
#endif
}

bool Cubos::started() const
{
    return mState != nullptr;
}

bool Cubos::shouldQuit() const
{
    return mWorld->resource<ShouldQuit>().value;
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

void Cubos::install(Plugin plugin)
{
    this->plugin(plugin);

    CUBOS_INFO("Installed plugin");
}

void Cubos::uninstall(Plugin plugin)
{
    // Check if the plugin is really installed.
    if (!mInstalledPlugins.contains(plugin))
    {
        CUBOS_ERROR("Couldn't uninstall plugin as it wasn't installed");
        return;
    }

    // Check if the plugin is still depended on by other plugins.
    if (mInstalledPlugins.at(plugin).dependentCount > 0)
    {
        CUBOS_ERROR("Couldn't uninstall plugin as it's still depended on by other plugins");
        return;
    }

    // Get a list of the types registered by the plugin.
    std::vector<const reflection::Type*> types{};
    for (const auto& [type, typePlugin] : mTypeToPlugin)
    {
        if (typePlugin == plugin)
        {
            types.push_back(type);
        }
    }

    // Unregister all of the types.
    for (const auto* type : types)
    {
        mTypeToPlugin.erase(*type);
        mWorld->unregister(*type);
    }

    // Get a list of the tags registered by the plugin.
    auto& info = mInstalledPlugins.at(plugin);
    std::vector<std::string> tags{};
    for (const auto& [tag, info] : mTags)
    {
        if (info.plugin == plugin)
        {
            tags.push_back(tag);
        }
    }

    // Remove all of the tags.
    for (const auto& tag : tags)
    {
        mTags.erase(tag);
    }

    // Remove all of the systems, conditions and planner tags.
    for (const auto& id : info.systems)
    {
        mSystemRegistry.remove(id);
    }

    for (const auto& id : info.conditions)
    {
        mSystemRegistry.remove(id);
    }

    for (const auto& id : info.tags)
    {
        mMainPlanner.remove(id);
    }

    // Remove all of the observers.
    for (const auto& id : mInstalledPlugins.at(plugin).observers)
    {
        mWorld->observers().unhook(id);
    }

    // Decrease dependent count of all dependencies.
    for (auto dependency : mInstalledPlugins.at(plugin).dependencies)
    {
        mInstalledPlugins.at(dependency).dependentCount -= 1;
    }

    // Recursively uninstall sub-plugins.
    for (auto subPlugin : mInstalledPlugins.at(plugin).subPlugins)
    {
        this->uninstall(subPlugin);
    }

    // Remove plugin from the list of installed plugins.
    mInstalledPlugins.erase(plugin);

    CUBOS_INFO("Uninstalled plugin");
}

Cubos::TagBuilder::TagBuilder(Cubos& cubos, std::string name, bool isStartup, Planner::TagId tagId)
    : mCubos{cubos}
    , mPlanner{isStartup ? cubos.mStartupPlanner : cubos.mMainPlanner}
    , mName{std::move(name)}
    , mIsStartup{isStartup}
    , mTagId{tagId}
{
}

Cubos::TagBuilder& Cubos::TagBuilder::before(const Tag& tag)
{
    CUBOS_ASSERT(mCubos.isRegistered(tag), "Tag {} wasn't registered by the plugin or its dependencies", tag.name());
    CUBOS_ASSERT(mCubos.mTags.at(tag.id()).isStartup == mIsStartup,
                 "Cannot make tag {} run before {}, as one is a startup tag and the other isn't", mName, tag.name());
    mPlanner.order(mTagId, mCubos.mTags.at(tag.id()).id);
    return *this;
}

Cubos::TagBuilder& Cubos::TagBuilder::after(const Tag& tag)
{
    CUBOS_ASSERT(mCubos.isRegistered(tag), "Tag {} wasn't registered by the plugin or its dependencies", tag.name());
    CUBOS_ASSERT(mCubos.mTags.at(tag.id()).isStartup == mIsStartup,
                 "Cannot make tag {} run after {}, as one is a startup tag and the other isn't", mName, tag.name());
    mPlanner.order(mCubos.mTags.at(tag.id()).id, mTagId);
    return *this;
}

Cubos::TagBuilder& Cubos::TagBuilder::tagged(const Tag& tag)
{
    CUBOS_ASSERT(mCubos.isRegistered(tag), "Tag {} wasn't registered by the plugin or its dependencies", tag.name());
    CUBOS_ASSERT(mCubos.mTags.at(tag.id()).isStartup == mIsStartup,
                 "Cannot tag {} with {}, as one is a startup tag and the other isn't", mName, tag.name());
    CUBOS_ASSERT(mPlanner.tag(mTagId, mCubos.mTags.at(tag.id()).id));
    return *this;
}

Cubos::TagBuilder& Cubos::TagBuilder::addTo(const Tag& tag)
{
    CUBOS_ASSERT(mCubos.isRegistered(tag), "Tag {} wasn't registered by the plugin or its dependencies", tag.name());
    CUBOS_ASSERT(mCubos.mTags.at(tag.id()).isStartup == mIsStartup,
                 "Cannot add tag {} to tag {}, as one is a startup tag and the other isn't", tag.name(), mName);
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
    auto dataTypeId = mCubos.mWorld->types().id(type);
    CUBOS_ASSERT(mCubos.mWorld->types().isComponent(dataTypeId), "Type {} isn't registered as a component",
                 type.name());

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

auto Cubos::SystemBuilder::withOpt(const reflection::Type& type, int target) && -> SystemBuilder&&
{
    CUBOS_ASSERT(mCubos.isRegistered(type), "No such component type {} was registered", type.name());
    auto dataTypeId = mCubos.mWorld->types().id(type);
    CUBOS_ASSERT(mCubos.mWorld->types().isComponent(dataTypeId), "Type {} isn't registered as a component",
                 type.name());

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

    mOptions.back().queryTerms.emplace_back(QueryTerm::makeOptComponent(dataTypeId, target));
    return std::move(*this);
}

auto Cubos::SystemBuilder::without(const reflection::Type& type, int target) && -> SystemBuilder&&
{
    CUBOS_ASSERT(mCubos.isRegistered(type), "No such component type {} was registered", type.name());
    auto dataTypeId = mCubos.mWorld->types().id(type);
    CUBOS_ASSERT(mCubos.mWorld->types().isComponent(dataTypeId), "Type {} isn't registered as a component",
                 type.name());

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
    auto dataTypeId = mCubos.mWorld->types().id(type);
    CUBOS_ASSERT(mCubos.mWorld->types().isRelation(dataTypeId), "Type {} isn't registered as a relation", type.name());

    CUBOS_ASSERT_IMP(traversal != Traversal::Random, mCubos.mWorld->types().isTreeRelation(dataTypeId),
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
            const auto& type = mCubos.mWorld->types().type(dataTypeId);
            CUBOS_ASSERT(mCubos.isRegistered(type), "Type {} isn't registered by the plugin or its dependencies",
                         type.name());
        }
    }

    for (const auto& dataTypeId : system.access().dataTypes)
    {
        const auto& type = mCubos.mWorld->types().type(dataTypeId);
        CUBOS_ASSERT(mCubos.isRegistered(type), "Type {} isn't registered by the plugin or its dependencies",
                     type.name());
    }

    CUBOS_DEBUG("Added system {}", mName);
    auto& planner = mIsStartup ? mCubos.mStartupPlanner : mCubos.mMainPlanner;
    auto systemId = mCubos.mSystemRegistry.add(mName, std::move(system));
    auto leafId = planner.add(mName, systemId);

    auto& pluginInfo = mCubos.mInstalledPlugins.at(mCubos.mPluginStack.back());
    pluginInfo.systems.push_back(systemId);

    if (!mIsStartup)
    {
        pluginInfo.tags.push_back(leafId);
    }

    if (mCondition)
    {
        auto conditionId = mCubos.mSystemRegistry.add(mName + "#condition", std::move(mCondition.value()));
        planner.onlyIf(leafId, conditionId);
        pluginInfo.conditions.push_back(conditionId);
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
    auto dataTypeId = mCubos.mWorld->types().id(type);
    CUBOS_ASSERT(mCubos.mWorld->types().isComponent(dataTypeId), "Type {} isn't registered as a component",
                 type.name());
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
    mDestroy = false;
    mColumnId = ColumnId::make(mCubos.mWorld->types().id(type));
    return std::move(*this);
}

auto Cubos::ObserverBuilder::onRemove(const reflection::Type& type, int target) && -> ObserverBuilder&&
{
    CUBOS_ASSERT(mCubos.isRegistered(type), "No such component type {} was registered", type.name());
    auto dataTypeId = mCubos.mWorld->types().id(type);
    CUBOS_ASSERT(mCubos.mWorld->types().isComponent(dataTypeId), "Type {} isn't registered as a component",
                 type.name());
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
    mDestroy = false;   
    mColumnId = ColumnId::make(mCubos.mWorld->types().id(type));
    return std::move(*this);
}

auto Cubos::ObserverBuilder::onDestroy(int target) && -> ObserverBuilder&&
{
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
    mDestroy = true;
    return std::move(*this);
}

auto Cubos::ObserverBuilder::onRelate(const reflection::Type& type, int fromTarget, int toTarget) && -> ObserverBuilder&&
{
    CUBOS_ASSERT(mCubos.isRegistered(type), "No such relation type {} was registered", type.name());
    auto dataTypeId = mCubos.mWorld->types().id(type);
    CUBOS_ASSERT(mCubos.mWorld->types().isRelation(dataTypeId), "Type {} isn't registered as a relation", type.name());
    CUBOS_ASSERT(mColumnId == ColumnId::Invalid, "An observer can only have at most one hook");

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
    mOptions.back().observedTarget = fromTarget;
    mRemove = false;
    mDestroy = false;
    mColumnId = ColumnId::make(mCubos.mWorld->types().id(type));
    return std::move(*this);
}

auto Cubos::ObserverBuilder::onUnrelate(const reflection::Type& type, int fromTarget, int toTarget) && -> ObserverBuilder&&
{
    CUBOS_ASSERT(mCubos.isRegistered(type), "No such relation type {} was registered", type.name());
    auto dataTypeId = mCubos.mWorld->types().id(type);
    CUBOS_ASSERT(mCubos.mWorld->types().isRelation(dataTypeId), "Type {} isn't registered as a relation", type.name());
    CUBOS_ASSERT(mColumnId == ColumnId::Invalid, "An observer can only have at most one hook");

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
    mOptions.back().observedTarget = fromTarget;
    mRemove = true;
    mDestroy = false;
    mColumnId = ColumnId::make(mCubos.mWorld->types().id(type));
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
    auto dataTypeId = mCubos.mWorld->types().id(type);
    CUBOS_ASSERT(mCubos.mWorld->types().isComponent(dataTypeId), "Type {} isn't registered as a component",
                 type.name());

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
    auto dataTypeId = mCubos.mWorld->types().id(type);
    CUBOS_ASSERT(mCubos.mWorld->types().isComponent(dataTypeId), "Type {} isn't registered as a component",
                 type.name());

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
    auto dataTypeId = mCubos.mWorld->types().id(type);
    CUBOS_ASSERT(mCubos.mWorld->types().isRelation(dataTypeId), "Type {} isn't registered as a relation", type.name());

    CUBOS_ASSERT_IMP(traversal != Traversal::Random, mCubos.mWorld->types().isTreeRelation(dataTypeId),
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
        const auto& type = mCubos.mWorld->types().type(dataTypeId);
        CUBOS_ASSERT(mCubos.isRegistered(type), "Type {} isn't registered by the plugin or its dependencies",
                     type.name());
    }

    CUBOS_ASSERT(mColumnId != ColumnId::Invalid, "You must set at least one hook for the observer");
    CUBOS_DEBUG("Added observer {}", mName);

    ObserverId id;
    if (mRemove)
    {
        id = mCubos.mWorld->observers().hookOnRemove(mColumnId, std::move(system));
    }
    else if (mDestroy)
    {
        id = mCubos.mWorld->observers().hookOnDestroy(mColumnId, std::move(system));
    }
    else
    {
        id = mCubos.mWorld->observers().hookOnAdd(mColumnId, std::move(system));
    }
    mCubos.mInstalledPlugins.at(mCubos.mPluginStack.back()).observers.push_back(id);
}
