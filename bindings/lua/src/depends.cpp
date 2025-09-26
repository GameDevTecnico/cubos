#include "systems.hpp"

#include "cubos.hpp"
#include "logging.hpp"

#include <cubos/core/ecs/cubos.hpp>
#include <cubos/core/ecs/plugin_queue.hpp>
#include <cubos/core/ecs/types.hpp>
#include <cubos/core/ecs/world.hpp>

#include <cubos/core/tel/logging.hpp>
#include <cubos/core/tel/level.hpp>

#include <unordered_map>
#include <vector>
#include <string>

using namespace cubos::core::ecs;
using namespace cubos::core::reflection;


static Plugin findPlugin(Cubos* cubos, const char* pluginName) {
    for (auto& entry : cubos->installedPlugins())
    {
        if (entry.second.name == pluginName)
        {
            return entry.first;
        }
    }

    return nullptr;
}

static PluginInfo getPluginInfo(Cubos* cubos, Plugin plugin) {
    return cubos->installedPlugins().at(plugin);
}

/*
 * Gets the contents of a plugin and its subplugins
 */
static void registerPlugin(Cubos* cubos, Plugin plugin, std::vector<const Type*>& pluginContent)
{
    PluginInfo pluginInfo = getPluginInfo(cubos, plugin);

    for (Plugin subplugin : pluginInfo.subPlugins)
    {
        registerPlugin(cubos, subplugin, pluginContent);
    }

    for (auto& type : pluginInfo.types)
    {
        pluginContent.push_back(type);
    }
}

namespace cubos::bindings::lua {
    int depends(lua_State *state)
    {
        Cubos* cubos = getCubos(state);
        World* world = cubos->world();
        Types types = world->types();
        std::vector<const Type*> pluginContent;

        const char* pluginName = luaL_checkstring(state, 1);
        Plugin plugin = findPlugin(cubos, pluginName);

        if (plugin == nullptr)
        {
            logLua(state, cubos::core::tel::Level::Error, "Unable to find plugin named " + std::string(pluginName));
            lua_pushnil(state);
            return 1;
        }

        registerPlugin(cubos, plugin, pluginContent);

        // The plugin is defined as a table with each type being a field.
        // Each type is defined as a userdata with a metatable corresponding
        // to its kind (Component/Resource/Relation).
        lua_newtable(state);

        for (auto& type : pluginContent)
        {
            const Type** instance = (const Type**) lua_newuserdata(state, sizeof(Type*));
            *instance = type;

            DataTypeId id = types.id(*type);

            if (types.isComponent(id))
            {
                luaL_getmetatable(state, "component");
            }
            else if (types.isResource(id))
            {
                luaL_getmetatable(state, "resource");
            }
            else if (types.isRelation(id))
            {
                luaL_getmetatable(state, "relation");
            }

            lua_setmetatable(state, -2);
            lua_setfield(state, -2, type->shortName().c_str());
        }

        return 1;
    }
}