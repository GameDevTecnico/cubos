#include "manager.hpp"
#include <thread>

#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/data/fs/standard_archive.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::data::FileSystem;
using cubos::core::data::StandardArchive;
using cubos::core::net::Address;

CUBOS_REFLECT_IMPL(tesseratos::ProjectManager::State)
{
    using namespace cubos::core::reflection;
    return Type::create("tesseratos::ProjectManager::State")
        .with(ConstructibleTrait::typed<State>().withMoveConstructor().build());
}

tesseratos::ProjectManager::ProjectManager(State& state, cubos::engine::Assets& assets, DebuggerSession& debugger)
    : mState(state)
    , mAssets(assets)
    , mDebugger(debugger)
{
}

bool tesseratos::ProjectManager::open(std::string projectOSPath, std::string binaryOSPath)
{
    if (projectOSPath.empty())
    {
        CUBOS_ERROR("Project path is empty");
        return false;
    }

    this->close();

    // Prepare the project directory for mounting.
    auto archive = std::make_unique<StandardArchive>(projectOSPath, /*isDirectory=*/true, /*readOnly=*/false);
    if (!archive->initialized())
    {
        CUBOS_ERROR("Could not open project at {}", projectOSPath);
        return false;
    }

    if (!FileSystem::mount("/project", std::move(archive)))
    {
        CUBOS_ERROR("Could not mount project archive to /project");
        return false;
    }

    // Load asset metadata from the project assets directory.
    mAssets.loadMeta("/project/assets");

    // We managed to open the project's directory, store the paths.
    mState.mProjectOSPath = std::move(projectOSPath);
    mState.mBinaryOSPath = std::move(binaryOSPath);

    // Try to launch the project.
    // If we can't, we can still keep the project open, but we won't be able to use its types.
    this->launch();

    return true;
}

bool tesseratos::ProjectManager::open() const
{
    return !mState.mProjectOSPath.empty();
}

void tesseratos::ProjectManager::close()
{
    if (mState.mProjectOSPath.empty())
    {
        return;
    }

    this->terminate();

    // Unload asset metadata from the project.
    mAssets.unloadMeta("/project/assets");

    // Unmount its directory from the virtual file system.
    if (!FileSystem::unmount("/project"))
    {
        CUBOS_ERROR("Failed to unmount previously open project directory");
    }

    mState.mProjectOSPath.clear();
}

bool tesseratos::ProjectManager::launch()
{
    // Stop the binary if it is already running.
    this->terminate();

    if (!mState.mProcess.start(mState.mBinaryOSPath, {"--debug", std::to_string(mState.mPort)}))
    {
        CUBOS_ERROR("Could not start project's process at {}", mState.mBinaryOSPath);
        return false;
    }

    // Try to connect to the child process's debugger.
    for (int i = 1, sleep = 100; i <= 3; ++i, sleep *= 2)
    {
        if (i > 0)
        {
            CUBOS_WARN("CCould not connect to project's process debugger in try {}, retrying in {} ms", i - 1, sleep);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        }

        if (mDebugger.connect(Address::LocalHost, mState.mPort))
        {
            CUBOS_INFO("Successfully connected to project's process debugger in try {}", i);
            break;
        }
    }

    // Check if we successfully connected to the child process's debugger.
    // If we didn't, kill the child process.
    if (!mDebugger.isConnected())
    {
        CUBOS_ERROR("Could not connect to the project's process debugger, terminating the process");
        mState.mProcess.kill();
        return false;
    }

    return true;
}

void tesseratos::ProjectManager::terminate()
{
    if (mDebugger.isConnected())
    {
        mDebugger.close(); // Issue a close command to the process' debugger.
    }
    else
    {
        mState.mProcess.kill();
    }

    mState.mProcess.wait(); // Wait for the process to finish.
}
