#include <cubos/bindings/lua/tel/logging_lua.hpp>

#include <cubos/api/core/tel/logging.h>
#include <lua.hpp>

#include <string>
#include <cstdlib>
#include <cstring>
#include <cmath>

/// @brief Formats all the elements in the lua stack into a string.
/// @param L Lua state.
/// @return String representation of the lua stack.
static char* format_args(lua_State *L)
{
    std::string str = "";
    int top = lua_gettop(L);

    for (int i = 1; i <= top; i++)
    {
        int type = lua_type(L, i);

        switch (type)
        {
        case LUA_TSTRING:
            str.append(lua_tostring(L, i));
            break;
        case LUA_TNUMBER: {
            lua_Number n = lua_tonumber(L, i);

            if (std::floor(n) == n) 
            {
                str.append(std::to_string(static_cast<int>(n)));
            }
            else
            {
                str.append(std::to_string(n));
            }
            break;
        }
        case LUA_TBOOLEAN:
            str.append(lua_toboolean(L, i) ? "true" : "false");
            break;
        case LUA_TNIL:
            str.append("nil");
            break;
        case LUA_TTABLE:
            str.append("<table>");
            break;
        case LUA_TFUNCTION:
            str.append("<function>");
            break;
        case LUA_TUSERDATA:
            str.append("<userdata>");
            break;
        case LUA_TLIGHTUSERDATA:
            str.append("<lightuserdata>");
            break;
        case LUA_TTHREAD:
            str.append("<thread>");
            break;
        }

        if (i != top) 
        {
            str.append(" ");
        }
    }
    
    char* c_str = (char*) malloc(sizeof(char) * (str.length() + 1));
    strcpy(c_str, str.c_str());

    return c_str;
}

int cubos::bindings::lua::logInfo(lua_State *L)
{
    char* str = format_args(L);
    cubos_tel_logging_info(str);
    return 0;
}

int cubos::bindings::lua::logWarn(lua_State *L) 
{
    char* str = format_args(L);
    cubos_tel_logging_warn(str);
    return 0;
}

int cubos::bindings::lua::logError(lua_State *L) 
{
    char* str = format_args(L);
    cubos_tel_logging_error(str);
    return 0;
}

int cubos::bindings::lua::logCritical(lua_State *L) 
{
    char* str = format_args(L);
    cubos_tel_logging_critical(str);
    return 0;
}
