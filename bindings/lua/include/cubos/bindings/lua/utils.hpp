/// @file
/// @brief Utils used for the lua custom functions.
/// @ingroup lua-bindings-plugin

#pragma once

#include <cubos/engine/prelude.hpp>
#include <lua.hpp>

#include <cubos/core/api.hpp>

using namespace cubos::core::ecs;

namespace cubos::bindings::lua
{
    /// @brief Adds a lua function to the cubos metatable.
    /// @param L Lua state.
    /// @param functionName Name of the function to be added.
    /// @param func Function.
    void addLuaFunction(lua_State *L, const char* functionName, int (*func)(lua_State*));

    Cubos* getCubosInstance(lua_State *L);

} // namespace cubos::bindings::lua
