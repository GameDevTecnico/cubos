/// @file
/// @brief Sets up the default types and traits for both the client and server sides of the reflection channel.

#pragma once

namespace cubos::core::reflection
{
    class TypeClient;
    class TypeServer;

    /// @brief Sets up the default types and traits for the client side of the reflection channel.
    void setupTypeClientDefaults(TypeClient& client);

    /// @brief Sets up the default types and traits for the server side of the reflection channel.
    void setupTypeServerDefaults(TypeServer& server);
} // namespace cubos::core::reflection
