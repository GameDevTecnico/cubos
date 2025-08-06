#include "systems.hpp"

#include <cubos/engine/prelude.hpp>

#include "cubos.hpp"

using cubos::bindings::lua::getCubos;
using cubos::bindings::lua::SystemType;
using cubos::engine::Cubos;

static int luaSystemCall(lua_State* state)
{
    luaL_checktype(state, 1, LUA_TTABLE);
    luaL_checktype(state, 2, LUA_TFUNCTION);
    int ref = luaL_ref(state, LUA_REGISTRYINDEX);

    Cubos* cubos = getCubos(state);

    lua_getfield(state, 1, "systemName");
    const char* systemName = lua_tostring(state, -1);
    lua_getfield(state, 1, "systemType");
    auto type = static_cast<SystemType>(lua_tonumber(state, -1));
    switch (type)
    {
    case SystemType::Startup:
        cubos->startupSystem(systemName).call([state, ref]() {
            lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
            lua_pcall(state, 0, 0, 0);
        });
        break;

    case SystemType::Update:
        cubos->system(systemName).call([state, ref]() {
            lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
            lua_pcall(state, 0, 0, 0);
        });
        break;
    }
    return 0;
}

static void pushSystem(lua_State* state, const char* systemName, SystemType type)
{
    lua_newtable(state);
    lua_pushstring(state, systemName);
    lua_setfield(state, -2, "systemName");
    lua_pushnumber(state, static_cast<int>(type));
    lua_setfield(state, -2, "systemType");
    lua_pushcfunction(state, luaSystemCall);
    lua_setfield(state, -2, "call");
}

namespace cubos::bindings::lua
{
    int startupSystem(lua_State* state)
    {
        luaL_checktype(state, 1, LUA_TSTRING);
        const char* systemName = lua_tostring(state, 1);
        pushSystem(state, systemName, SystemType::Startup);
        return 1;
    };

    int system(lua_State* state)
    {
        luaL_checktype(state, 1, LUA_TSTRING);
        const char* systemName = lua_tostring(state, 1);
        pushSystem(state, systemName, SystemType::Update);
        return 1;
    };
} // namespace cubos::bindings::lua
