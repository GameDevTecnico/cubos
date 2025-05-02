/// @file
/// @brief System argument @ref tesseratos::ProjectManager.
/// @ingroup tesseratos-debugger-plugin

#pragma once

#include <string>

#include <cubos/core/ecs/system/fetcher.hpp>
#include <cubos/core/thread/process.hpp>

#include <cubos/engine/assets/plugin.hpp>

#include "../debugger/debugger.hpp"

namespace tesseratos
{
    /// @brief System argument which can be used to manage the currently loaded project.
    class ProjectManager
    {
    public:
        CUBOS_REFLECT;

        /// @brief Resource which holds the state of the project manager.
        class State;

        /// @brief Constructs.
        /// @param state State of the project manager.
        /// @param assets Asset manager.
        /// @param debugger Debugger.
        ProjectManager(State& state, cubos::engine::Assets& assets, DebuggerSession& debugger);

        /// @brief Opens a project directory.
        ///
        /// The given project directory is mounted in the /project directory in the virtual file system.
        /// Unmounts any previously opened project directory.
        ///
        /// @param projectOSPath Project's directory path in the operating system.
        /// @param binaryOSPath Project's binary directory path in the operating system.
        /// @return Whether the project could be opened successfully.
        bool open(std::string projectOSPath, std::string binaryOSPath);

        /// @brief Checks whether a project is currently open.
        /// @return Whether a project is currently open.
        bool open() const;

        /// @brief Closes the currently open project.
        void close();

        /// @brief Launches the project's binary and attaches the debugger.
        ///
        /// If the binary is already running, it is stopped first.
        ///
        /// @return Whether the project could be launched successfully.
        bool launch();

        /// @brief Stops the project's binary.
        void terminate();

    private:
        State& mState;
        cubos::engine::Assets& mAssets;
        DebuggerSession& mDebugger;
    };

    class ProjectManager::State
    {
    public:
        CUBOS_REFLECT;

        /// @brief Default constructs.
        State() = default;

    private:
        friend ProjectManager;

        std::string mProjectOSPath{};
        std::string mBinaryOSPath{};
        cubos::core::thread::Process mProcess{};
        uint16_t mPort{9335};
    };
} // namespace tesseratos

namespace cubos::core::ecs
{
    template <>
    class SystemFetcher<tesseratos::ProjectManager>
    {
    public:
        static inline constexpr bool ConsumesOptions = false;

        SystemFetcher<tesseratos::ProjectManager::State&> state;
        SystemFetcher<cubos::engine::Assets&> assets;
        SystemFetcher<tesseratos::DebuggerSession&> debugger;

        SystemFetcher(World& world, const SystemOptions& options)
            : state{world, options}
            , assets{world, options}
            , debugger{world, options}
        {
        }

        void analyze(SystemAccess& access) const
        {
            state.analyze(access);
            assets.analyze(access);
            debugger.analyze(access);
        }

        tesseratos::ProjectManager fetch(const SystemContext& ctx)
        {
            return {state.fetch(ctx), assets.fetch(ctx), debugger.fetch(ctx)};
        }
    };
} // namespace cubos::core::ecs
