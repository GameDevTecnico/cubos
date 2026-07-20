#include <algorithm>
#include <string>

#include <cubos/bindings/lua/plugin.hpp>
#include <lua.hpp>

#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/data/fs/standard_archive.hpp>
#include <cubos/core/ecs/system/arguments/plugins.hpp>

#include <cubos/engine/settings/plugin.hpp>

#include "cubos.hpp"
#include "cubos/engine/prelude.hpp"

using namespace cubos::engine;
using cubos::core::data::FileSystem;
using cubos::core::data::StandardArchive;
using cubos::core::ecs::Plugins;

static const std::string ScriptsMountPoint = "/scripts/lua";

static void loadScripts(std::string_view path, lua_State* state)
{
    auto directory = FileSystem::find(path);
    if (directory != nullptr && directory->directory())
    {
        auto child = directory->child();
        while (child != nullptr)
        {
            if (child->directory())
            {
                loadScripts(child->path(), state);
            }
            else if (child->name().ends_with(".lua"))
            {
                std::string moduleName = std::string{child->path().substr(
                    ScriptsMountPoint.length() + 1, child->path().length() - 5 - ScriptsMountPoint.length())};
                std::ranges::replace(moduleName, '/', '.');
                std::string execString = std::string{"require('"} + moduleName + "')";
                luaL_dostring(state, execString.c_str());
            }
            child = child->sibling();
        }
    }
}

static int searchModule(lua_State* state)
{
    const char* moduleName = luaL_checkstring(state, 1);
    std::string module = moduleName;
    std::ranges::replace(module, '.', '/');
    module = module + ".lua";

    lua_pushstring(state, "scriptsPath");
    lua_gettable(state, LUA_REGISTRYINDEX);
    std::string filePath = lua_tostring(state, -1);
    filePath.push_back('/');
    filePath.append(module);

    auto script = FileSystem::find(filePath);
    if (script != nullptr && !script->directory() && script->name().ends_with(".lua"))
    {
        std::string contents;
        {
            auto stream = script->open(cubos::core::data::File::OpenMode::Read);
            stream->readUntil(contents, nullptr);
        }
        luaL_loadstring(state, contents.c_str());
        return 1;
    }
    CUBOS_ERROR("Lua module {} not found!", moduleName);
    lua_pushnil(state);
    return 1;
}

static void setupCustomLoader(lua_State* state)
{
    lua_pushstring(state, "scriptsPath");
    lua_pushstring(state, ScriptsMountPoint.c_str());
    lua_settable(state, LUA_REGISTRYINDEX);

    lua_getglobal(state, "package");
    lua_newtable(state);
    lua_pushcfunction(state, searchModule);
    lua_rawseti(state, -2, 1);
    lua_setfield(state, -2, "loaders");
    lua_pop(state, lua_gettop(state));
}

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        lua_State* luaState;

        State(Cubos& cubos)
            : luaState(luaL_newstate())
        {
            luaL_openlibs(luaState);
            setupCustomLoader(luaState);
            cubos::bindings::lua::injectCubos(luaState, &cubos);
        }

        ~State()
        {
            lua_close(luaState);
        }
    };
} // namespace
//
void cubos::bindings::lua::luaBindingsPlugin(Cubos& cubos)
{
    cubos.depends(settingsPlugin);

    cubos.resource<State>(cubos);

    cubos.startupSystem("load lua scripts").after(settingsTag).call([](Settings& settings, Plugins plugins) {
        std::string scriptsPath = settings.getString("scripts.lua.app.osPath", "");
        if (!scriptsPath.empty())
        {
            auto archive = std::make_unique<StandardArchive>(scriptsPath, true, true);
            if (archive->initialized() && FileSystem::mount(ScriptsMountPoint, std::move(archive)))
            {
                CUBOS_INFO("Mounted lua scripts directory with path {}", scriptsPath);
            }
            else
            {
                CUBOS_ERROR("Couldn't mount scripts directory with path {}", scriptsPath);
            }
            (void)plugins;
            plugins.add([](Cubos& other) {
                auto& state = other.world()->resource<State>();
                loadScripts(ScriptsMountPoint, state.luaState);
            });
        }
    });
}
