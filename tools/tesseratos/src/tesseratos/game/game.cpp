#include "game.hpp"

#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

using tesseratos::Game;

CUBOS_REFLECT_IMPL(tesseratos::Game)
{
    using namespace cubos::core::reflection;
    return Type::create("tesseratos::Game").with(ConstructibleTrait::typed<Game>().build());
}

void Game::inject(cubos::core::ecs::Plugin target, cubos::core::ecs::Plugin plugin)
{
    CUBOS_ASSERT(!mInjections.contains(target), "Target plugin was injected into");
    mInjections.emplace(target, plugin);
}

void Game::resource(cubos::core::memory::AnyValue resource)
{
    CUBOS_ASSERT(!mCubos.isStarted(), "Game has already been started");
    mResources.emplace_back(std::move(resource));
}

void Game::unload()
{
    mCubos.reset();
    mPlugin = nullptr;
}

void Game::load(cubos::core::ecs::Plugin plugin)
{
    this->unload();
    mPlugin = plugin;

    for (auto [target, inject] : mInjections)
    {
        mCubos.inject(target, inject);
    }

    CUBOS_INFO("Loading game");
    mCubos.plugin(mPlugin);
}

void Game::reset()
{
    this->load(mPlugin);
}

void Game::start()
{
    CUBOS_ASSERT(mPlugin != nullptr, "Game has not been loaded");
    CUBOS_ASSERT(!mCubos.isStarted(), "Game has already been started");

    for (const auto& resource : mResources)
    {
        if (mCubos.world().types().contains(resource.type()))
        {
            mCubos.world().insertResource(resource);
        }
        else
        {
            CUBOS_WARN("Resource {} not registered on Game, skipping insertion", resource.type().name());
        }
    }

    // Switch to the game's context.
    for (auto& func : mContextSwitches)
    {
        func(true);
    }

    CUBOS_INFO("Starting game");
    mCubos.start();
    mPaused = false;

    // Restore the editor context.
    for (auto& func : mContextSwitches)
    {
        func(false);
    }
}

void Game::update()
{
    CUBOS_ASSERT(mCubos.isStarted(), "Game has not been started");

    if (!mPaused)
    {
        // Switch to the game's context.
        for (auto& func : mContextSwitches)
        {
            func(true);
        }

        if (mCubos.update())
        {
            CUBOS_INFO("Game has ended, resetting");
            this->reset();
        }

        // Restore the editor context.
        for (auto& func : mContextSwitches)
        {
            func(false);
        }
    }
}

void Game::pause()
{
    CUBOS_ASSERT(mCubos.isStarted(), "Game has not been started");

    if (!mPaused)
    {
        CUBOS_INFO("Pausing game");
        mPaused = true;
    }
}

void Game::resume()
{
    CUBOS_ASSERT(mCubos.isStarted(), "Game has not been started");

    if (mPaused)
    {
        CUBOS_INFO("Resuming game");
        mPaused = false;
    }
}

bool Game::isLoaded() const
{
    return mPlugin != nullptr;
}

bool Game::isStarted() const
{
    return this->isLoaded() && mCubos.isStarted();
}

bool Game::isRunning() const
{
    return this->isStarted() && !mPaused;
}

cubos::core::ecs::World& Game::world()
{
    CUBOS_ASSERT(this->isLoaded(), "Game has not been loaded");
    return mCubos.world();
}

void Game::addContextSwitch(cubos::core::memory::Function<void(bool)> func)
{
    mContextSwitches.emplace_back(std::move(func));
}
