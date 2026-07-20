/// @dir
/// @brief @ref lua-bindings-plugin plugin directory.

/// @file
/// @brief Logging lua api.
/// @ingroup lua-bindings-plugin
#pragma once

#include <cstdint>
#include <string>
#include <cubos/core/tel/level.hpp>

#include <lua.hpp>

namespace cubos::bindings::lua
{
    /// @brief Logs a message showing lua specific info.
    /// @param state The current lua state.
    /// @param level Logging level.
    /// @param message Message to print.
    /// @ingroup lua-bindings-plugin
    void logLua(lua_State* state, cubos::core::tel::Level level, std::string message);

    /// @brief Lua api function to log information.
    /// @param state The current lua state.
    /// @ingroup lua-bindings-plugin
    int logInfo(lua_State* state);

    /// @brief Lua api function to log a warning.
    /// @param state The current lua state.
    /// @ingroup lua-bindings-plugin
    int logWarn(lua_State* state);

    /// @brief Lua api function log an error.
    /// @param state The current lua state.
    /// @ingroup lua-bindings-plugin
    int logError(lua_State* state);
} // namespace cubos::bindings::lua