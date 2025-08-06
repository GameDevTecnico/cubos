#include <string_view>

#include <cubos/core/reflection/external/string_view.hpp>
#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/prelude.hpp>

#include "cubos.hpp"
#include "systems.hpp"

using cubos::engine::Cubos;

static void pushFunction(lua_State* state, const char* name, lua_CFunction fn)
{
    lua_pushcfunction(state, fn);
    lua_setfield(state, -2, name);
}

void cubos::bindings::lua::injectCubos(lua_State* state, Cubos* cubos)
{
    lua_pushstring(state, "cubos");
    lua_pushlightuserdata(state, cubos);
    lua_settable(state, LUA_REGISTRYINDEX);

    lua_newtable(state);
    pushFunction(state, "startupSystem", startupSystem);
    pushFunction(state, "system", system);
    lua_setglobal(state, "cubos");
}

Cubos* cubos::bindings::lua::getCubos(lua_State* state)
{
    lua_pushstring(state, "cubos");
    lua_gettable(state, LUA_REGISTRYINDEX);
    return static_cast<Cubos*>(lua_touserdata(state, -1));
}
