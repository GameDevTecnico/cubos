/// @dir
/// @brief @ref lua-bindings-plugin plugin directory.

/// @file
/// @brief Systems lua api.
/// @ingroup lua-bindings-plugin
#pragma once

#include <cstdint>

#include <lua.hpp>

namespace cubos::bindings::lua
{
    /// @brief Possible system types that can be registered
    /// @ingroup lua-bindings-plugin
    enum class SystemType : uint8_t
    {
        Startup = 0,
        Update = 1
    };

    /// @brief Lua api function to register a startup system (executed only once).
    /// @param state The current lua state.
    /// @ingroup lua-bindings-plugin
    int startupSystem(lua_State* state);

    /// @brief Lua api function to register a system (executed more than once).
    /// @param state The current lua state.
    /// @ingroup lua-bindings-plugin
    int system(lua_State* state);
} // namespace cubos::bindings::lua
