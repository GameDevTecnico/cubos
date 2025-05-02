#include "debugger.hpp"

#include <cubos/core/data/des/binary.hpp>
#include <cubos/core/data/ser/binary.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::data::BinaryDeserializer;
using cubos::core::data::BinarySerializer;
using cubos::core::memory::Opt;
using cubos::core::net::Address;
using cubos::core::net::TcpStream;
using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::Type;
using cubos::core::reflection::TypeClient;
using tesseratos::DebuggerSession;

CUBOS_REFLECT_IMPL(DebuggerSession)
{
    return Type::create("tesseratos::DebuggerSession")
        .with(ConstructibleTrait::typed<DebuggerSession>().withDefaultConstructor().withMoveConstructor().build());
}

TypeClient& DebuggerSession::typeClient()
{
    return mTypeClient;
}

const TypeClient& DebuggerSession::typeClient() const
{
    return mTypeClient;
}

bool DebuggerSession::isConnected() const
{
    return mConnection.contains();
}

bool DebuggerSession::connect(const Address& address, uint16_t port)
{
    this->disconnect();

    if (!mStream.connect(address, port))
    {
        CUBOS_ERROR("Failed to connect to debugger at {}:{}", address, port);
        return false;
    }

    CUBOS_INFO("Connected to debugger at {}:{}", address, port);

    if (auto connection = mTypeClient.connect(mStream))
    {
        CUBOS_INFO("Received shared reflection data from debugger successfully");
        mConnection.replace(std::move(*connection));
        return true;
    }

    CUBOS_ERROR("Failed to receive shared reflection data from debugger, closing connection");
    mStream.disconnect();
    return false;
}

void DebuggerSession::disconnect()
{
    if (mConnection.contains())
    {
        CUBOS_INFO("Disconnecting from debugger");
        BinarySerializer{mStream}.write<const char*>("disconnect");
        mStream.disconnect();
        mConnection.reset();
    }
}

const TypeClient::Connection& DebuggerSession::connection() const
{
    CUBOS_ASSERT(mConnection.contains(), "Debugger is not connected");
    return mConnection.value();
}

bool DebuggerSession::run()
{
    if (!mConnection.contains())
    {
        CUBOS_ERROR("Cannot issue start command, not connected to debugger");
        return false;
    }

    if (!BinarySerializer{mStream}.write<const char*>("run"))
    {
        CUBOS_ERROR("Failed to serialize run command");
        this->disconnect();
        return false;
    }

    return true;
}

bool DebuggerSession::pause()
{
    if (!mConnection.contains())
    {
        CUBOS_ERROR("Cannot issue pause command, not connected to debugger");
        return false;
    }

    if (!BinarySerializer{mStream}.write<const char*>("pause"))
    {
        CUBOS_ERROR("Failed to serialize pause command");
        this->disconnect();
        return false;
    }

    return true;
}

bool DebuggerSession::update(std::size_t count)
{
    if (!mConnection.contains())
    {
        CUBOS_ERROR("Cannot issue update command, not connected to debugger");
        return false;
    }

    BinarySerializer ser{mStream};
    if (!ser.write<const char*>("update") || !ser.write(count))
    {
        CUBOS_ERROR("Failed to serialize update command");
        this->disconnect();
        return false;
    }

    return true;
}

bool DebuggerSession::close()
{
    if (!mConnection.contains())
    {
        CUBOS_ERROR("Cannot issue close command, not connected to debugger");
        return false;
    }

    BinarySerializer ser{mStream};
    if (!ser.write<const char*>("close"))
    {
        CUBOS_ERROR("Failed to serialize close command");
        this->disconnect();
        return false;
    }

    CUBOS_INFO("Sent close command, disconnecting from debugger");
    mStream.disconnect();
    mConnection.reset();
    return true;
}