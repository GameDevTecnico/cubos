/// @dir
/// @brief @ref lua-bindings-plugin plugin directory.

/// @file
/// @brief Logging lua api.
/// @ingroup lua-bindings-plugin
#pragma once

#include <cstdint>

#include <lua.hpp>

namespace cubos::bindings::lua
{
    /// @brief Lua api function that gets the components, resources and relations of a given plugin
    //  and its subplugins.
    /// @param state The current lua state.
    /// @ingroup lua-bindings-plugin
    int depends(lua_State* state);

} // namespace cubos::bindings::lua