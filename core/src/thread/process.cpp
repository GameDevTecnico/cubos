#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/tel/logging.hpp>
#include <cubos/core/thread/process.hpp>

using cubos::core::thread::Process;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <csignal>
#include <cstring>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

CUBOS_REFLECT_IMPL(Process)
{
    return reflection::Type::create("cubos::core::thread::Process")
        .with(reflection::ConstructibleTrait::typed<Process>().withDefaultConstructor().withMoveConstructor().build());
}

Process::~Process()
{
    this->kill();
    this->wait();
}

Process::Process(Process&& other) noexcept
    : mHandle{other.mHandle}
{
    other.mHandle = nullptr;
}

Process& Process::operator=(Process&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    this->wait();
    mHandle = other.mHandle;
    other.mHandle = nullptr;
    return *this;
}

bool Process::start(const std::string& command, const std::vector<std::string>& args, const std::string& cwd)
{
    this->wait();

#ifdef _WIN32
    // Launch the binary with appropriate arguments.
    std::string finalCommand = command;
    for (const auto& arg : args)
    {
        finalCommand += " " + arg;
    }
    STARTUPINFOA si = {sizeof(si)};
    PROCESS_INFORMATION pi;
    if (!CreateProcessA(nullptr, const_cast<char*>(finalCommand.c_str()), nullptr, nullptr, FALSE, 0, nullptr,
                        cwd.empty() ? nullptr : cwd.c_str(), &si, &pi))
    {
        CUBOS_ERROR("Failed to start process {} with error {}", command, GetLastError());
        return false;
    }

    mHandle = new PROCESS_INFORMATION{pi};
    CUBOS_INFO("Started process {} with PID {}", command, pi.dwProcessId);
#else
    auto pid = fork();
    if (pid == -1)
    {
        CUBOS_ERROR("Failed to fork process");
        return false;
    }

    if (pid == 0) // Are we the child process?
    {
        if (!cwd.empty())
        {
            // Change the working directory.
            if (chdir(cwd.c_str()) == -1)
            {
                CUBOS_CRITICAL("Failed to change working directory to {}", cwd);
                exit(1);
            }
        }

        // Launch the binary with appropriate arguments.
        std::vector<std::string> argsCopy = args;
        argsCopy.insert(argsCopy.begin(), command);
        std::vector<char*> argv{};
        argv.reserve(argsCopy.size() + 1);
        for (const auto& arg : argsCopy)
        {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
        execvp(command.c_str(), argv.data());

        // If we reach this point, execv failed. Get the error message and log it.
        CUBOS_CRITICAL("Failed to start process {} with error {}", command, strerror(errno));
        exit(1);
    }

    // We are the parent process.
    mHandle = new pid_t{pid};
    CUBOS_INFO("Started process {} with PID {}", command, pid);
#endif

    return true;
}

void Process::kill()
{
    if (mHandle == nullptr)
    {
        return;
    }

#ifdef _WIN32
    auto* pi = static_cast<PROCESS_INFORMATION*>(mHandle);
    ::TerminateProcess(pi->hProcess, 1);
    CUBOS_DEBUG("Process {} killed", pi->dwProcessId);
#else
    auto* pid = static_cast<pid_t*>(mHandle);
    ::kill(*pid, SIGKILL);
    CUBOS_DEBUG("Process {} killed", *pid);
#endif
}

bool Process::wait()
{
    int status;
    return this->wait(status);
}

bool Process::wait(int& status)
{
    if (mHandle == nullptr)
    {
        return false;
    }

#ifdef _WIN32
    auto* pi = static_cast<PROCESS_INFORMATION*>(mHandle);
    mHandle = nullptr;

    ::WaitForSingleObject(pi->hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(pi->hProcess, &exitCode);
    status = static_cast<int>(exitCode);
    CUBOS_DEBUG("Process {} exited with status {}", pi->dwProcessId, status);

    ::CloseHandle(pi->hProcess);
    ::CloseHandle(pi->hThread);
    delete pi;

    return true;
#else
    auto pid = *static_cast<pid_t*>(mHandle);
    delete static_cast<pid_t*>(mHandle);
    mHandle = nullptr;

    int waitStatus;
    ::waitpid(pid, &waitStatus, 0);

    if (WIFEXITED(waitStatus))
    {
        status = WEXITSTATUS(waitStatus);
        CUBOS_DEBUG("Process {} exited with status {}", pid, status);
        return true;
    }

    if (WIFSIGNALED(waitStatus))
    {
        CUBOS_WARN("Process {} terminated by signal {}", pid, WTERMSIG(waitStatus));
        return false;
    }

    CUBOS_WARN("Process {} terminated abnormally", pid);
    return false;
#endif
}

bool Process::started() const
{
    return mHandle != nullptr;
}
