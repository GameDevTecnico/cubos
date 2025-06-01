#include <cubos/bindings/lua/utils.hpp>
#include <lua.hpp>
#include <cubos/core/api.hpp>

using namespace cubos::core::ecs;

void cubos::bindings::lua::addLuaFunction(lua_State *L, const char* functionName, int (*func)(lua_State*))
{
    luaL_getmetatable(L, "cubos");
    lua_pushcfunction(L, func);
    lua_setfield(L, -2, functionName);
    lua_pop(L, 1);
}

Cubos* cubos::bindings::lua::getCubosInstance(lua_State *L)
{
    lua_getfield(L, LUA_REGISTRYINDEX, "instance");
    Cubos** instance = (Cubos**) lua_touserdata(L, -1);

    return *instance;
}
