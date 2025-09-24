#include "systems.hpp"

#include <string>
#include <cmath>

#include <cubos/engine/prelude.hpp>

using namespace cubos::core::tel;

static std::string formatArgs(lua_State *state)
{
    std::string str;
    int top = lua_gettop(state);

    for (int i = 1; i <= top; i++)
    {
        int type = lua_type(state, i);

        switch (type)
        {
        case LUA_TSTRING:
            str.append(lua_tostring(state, i));
            break;
        case LUA_TNUMBER: {
            lua_Number n = lua_tonumber(state, i);

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
            str.append(lua_toboolean(state, i) ? "true" : "false");
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
            str.append(" ");
    }

    return str;
}

static void log(lua_State* state, Level logLevel)
{
    std::string formattedArgs = formatArgs(state);
    Logger::Location location;
    lua_Debug debugInfo;

    if (lua_getstack(state, 1, &debugInfo))
    {
        if (lua_getinfo(state, "nSl", &debugInfo))
        {
            location.function = debugInfo.name ? debugInfo.name : "undefined";
            location.file = debugInfo.short_src;
            location.line = debugInfo.currentline;

            Logger::write(logLevel, location, formattedArgs);
        }
    }
}

namespace cubos::bindings::lua
{
    int logInfo(lua_State* state)
    {
        log(state, Level::Info);
        return 0;
    };

    int logWarn(lua_State* state)
    {
        log(state, Level::Warn);
        return 0;
    };

    int logError(lua_State* state)
    {
        log(state, Level::Error);
        return 0;
    };
} // namespace cubos::bindings::lua
