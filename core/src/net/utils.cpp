#include <cstring>

#include <cubos/core/net/utils.hpp>

std::string cubos::core::net::logSystemError()
{
#ifdef _WIN32
    static char message[256] = {0};
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, WSAGetLastError(), 0, message, 256, 0);
    char* nl = strrchr(message, '\n');
    if (nl)
    {
        *nl = 0;
    }
    return {message};
#else
    return {std::strerror(errno)};
#endif
}