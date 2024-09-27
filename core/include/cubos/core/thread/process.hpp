/// @file
/// @brief Class @ref cubos::core::thread::Process.
/// @ingroup core-thread

#pragma once

#include <string>
#include <vector>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::thread
{
    /// @brief Provides a cross-platform way to spawn child processes.
    /// @ingroup core-thread
    class Process final
    {
    public:
        CUBOS_REFLECT;

        ~Process();

        /// @brief Default constructor.
        Process() = default;

        /// @brief Move constructor.
        Process(Process&& other) noexcept;

        /// @brief Move assignment operator.
        Process& operator=(Process&& other) noexcept;

        /// @brief Starts a new process.
        /// @param command Command to execute.
        /// @param args Arguments to pass to the command.
        /// @param cwd Working directory for the new process.
        /// @return Whether the process was started successfully.
        bool start(const std::string& command, const std::vector<std::string>& args = {}, const std::string& cwd = "");

        /// @brief Kills the process.
        void kill();

        /// @brief Waits for the process to finish.
        /// @return Whether the process exited normally.
        bool wait();

        /// @brief Waits for the process to finish.
        /// @param status Exit code of the process, if it exited normally.
        /// @return Whether the process exited normally.
        bool wait(int& status);

        /// @brief Checks whether the process has been started.
        /// @return Whether the process has been started.
        bool started() const;

    private:
        void* mHandle{nullptr};
    };
} // namespace cubos::core::thread
