/// @file
/// @brief Plugin entry point.
/// @ingroup lua-bindings-logging

#pragma once

#include <lua.hpp>

namespace cubos::bindings::lua
{
    /// @brief Used for logging information which is useful in release builds.
    ///
    /// Lua template function.
    int logInfo(lua_State *L);

    /// @brief Used for logging unexpected events.
    ///
    /// Lua template function.
    int logWarn(lua_State *L);

    /// @brief Used for logging recoverable errors.
    ///
    /// Lua template function.
    int logError(lua_State *L);

    /// @brief Used for logging unrecoverable errors.
    ///
    /// Lua template function.
    int logCritical(lua_State *L);

} // namespace cubos::bindings::lua
