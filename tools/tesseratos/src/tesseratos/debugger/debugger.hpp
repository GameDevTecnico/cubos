/// @file
/// @brief Resource @ref tesseratos::DebuggerSession.
/// @ingroup tesseratos-debugger-plugin

#pragma once

#include <cubos/core/net/tcp_stream.hpp>
#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/type_client.hpp>

namespace tesseratos
{
    class DebuggerSession
    {
    public:
        CUBOS_REFLECT;

        /// @brief Returns the type client.
        /// @return Type client.
        cubos::core::reflection::TypeClient& typeClient();

        /// @brief Returns the type client.
        /// @return Type client.
        const cubos::core::reflection::TypeClient& typeClient() const;

        /// @brief Checks if the debugger is connected.
        /// @return Whether the debugger is connected.
        bool isConnected() const;

        /// @brief Connects to a debugger.
        /// @param address Address of the debugger.
        /// @param port Port of the debugger.
        /// @return Whether the connection was successful.
        bool connect(const cubos::core::net::Address& address, uint16_t port);

        /// @brief Disconnects from the debugger, if connected.
        void disconnect();

        /// @brief Returns the connection information.
        /// @note Aborts if the debugger is not connected.
        /// @return Connection information.
        const cubos::core::reflection::TypeClient::Connection& connection() const;

        /// @brief Issues a run command to the connected debugger.
        /// @return Whether the command was sent successfully.
        bool run();

        /// @brief Issues a pause command to the connected debugger.
        /// @return Whether the command was sent successfully.
        bool pause();

        /// @brief Issues an update command to the connected debugger.
        /// @param count Number of frames to update.
        /// @return Whether the command was sent successfully.
        bool update(std::size_t count);

        /// @brief Issues a close command to the connected debugger.
        /// @return Whether the command was sent successfully.
        bool close();

    private:
        cubos::core::reflection::TypeClient mTypeClient;
        cubos::core::memory::Opt<cubos::core::reflection::TypeClient::Connection> mConnection;
        cubos::core::net::TcpStream mStream;
    };
} // namespace tesseratos