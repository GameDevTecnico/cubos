/// @file
/// @brief Network utilities.
/// @ingroup core-net

#pragma once

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

#include <string>

namespace cubos::core::net
{
    /// @brief Socket type. Differs from Windows (SOCKET) and POSIX (int).
    using InnerSocket = decltype(socket(0, 0, 0));

    /// @brief Represents an invalid socket.
    constexpr InnerSocket InnerInvalidSocket = static_cast<InnerSocket>(-1);

    /// @brief Cross-platform utility to get last networking error message.
    /// @return Message.
    std::string logSystemError();

} // namespace cubos::core::net
