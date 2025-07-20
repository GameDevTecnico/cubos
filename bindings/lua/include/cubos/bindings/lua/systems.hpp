#pragma once

#include <lua.hpp>

namespace cubos::bindings::lua
{
    enum class SystemType
    {
        STARTUP = 0,
        UPDATE = 1
    };

    int startupSystem(lua_State* state);

    int system(lua_State* state);
} // namespace cubos::bindings::lua
