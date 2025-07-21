#pragma once

#include <lua.hpp>

#include <cubos/engine/prelude.hpp>

namespace cubos::bindings::lua
{
    void injectCubos(lua_State* state, cubos::engine::Cubos* cubos);

    cubos::engine::Cubos* getCubos(lua_State* state);
} // namespace cubos::bindings::lua
