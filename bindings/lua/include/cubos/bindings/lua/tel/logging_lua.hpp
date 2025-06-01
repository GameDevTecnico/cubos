/// @file
/// @brief Logging functions for lua.
/// @ingroup lua-bindings-tel

#pragma once

#include <lua.hpp>

namespace cubos::bindings::lua
{
    /// @brief Adds logging functions to the cubos metatable.
    /// @param L Lua state.
    ///
    /// ## Functions added
    /// - cubos.info(), logs information.
    /// - cubos.warn(), logs warnings.
    /// - cubos.error(), logs errors.
    /// - cubos.critical(), logs critical errors.
    void addLoggingFunctions(lua_State *L);

} // namespace cubos::bindings::lua
