#include <string_view>

#include <cubos/core/reflection/external/string_view.hpp>
#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/prelude.hpp>

#include "cubos.hpp"
#include "systems.hpp"
#include "logging.hpp"
#include "depends.hpp"

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

    // Ecs
    pushFunction(state, "startupSystem", startupSystem);
    pushFunction(state, "system", system);
    pushFunction(state, "depends", depends);

    // Logging
    pushFunction(state, "info", logInfo);
    pushFunction(state, "warn", logWarn);
    pushFunction(state, "error", logError);

    lua_setglobal(state, "cubos");


    // Create metatables for types
    luaL_newmetatable(state, "component");
    lua_pushstring(state, "Component");
    lua_setfield(state, -2, "__kind");
    lua_setfield(state, -1, "__index");

    luaL_newmetatable(state, "resource");
    lua_pushstring(state, "Resource");
    lua_setfield(state, -2, "__kind");
    lua_setfield(state, -1, "__index");

    luaL_newmetatable(state, "relation");
    lua_pushstring(state, "Relation");
    lua_setfield(state, -2, "__kind");
    lua_setfield(state, -1, "__index");
}

Cubos* cubos::bindings::lua::getCubos(lua_State* state)
{
    lua_pushstring(state, "cubos");
    lua_gettable(state, LUA_REGISTRYINDEX);
    return static_cast<Cubos*>(lua_touserdata(state, -1));
}
