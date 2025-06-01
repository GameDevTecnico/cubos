#include <cubos/bindings/lua/plugin.hpp>
#include <cubos/bindings/lua/tel/logging_lua.hpp>
#include <lua.hpp>

#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/data/fs/standard_archive.hpp>

#include <cubos/engine/settings/plugin.hpp>

using namespace cubos::engine;
using cubos::core::data::FileSystem;
using cubos::core::data::StandardArchive;

static constexpr std::string_view ScriptsMountPoint = "/scripts/lua";

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        lua_State* luaState;

        State(lua_State* state)
            : luaState(state)
        {
        }

        ~State()
        {
            lua_close(luaState);
        }
    };
} // namespace

static void loadScripts(std::string_view path, State& state)
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
                std::string contents;
                {
                    auto stream = child->open(cubos::core::data::File::OpenMode::Read);
                    stream->readUntil(contents, nullptr);
                }
                luaL_dostring(state.luaState, contents.c_str());
            }
            child = child->sibling();
        }
    }
}

static void addLuaFunction(const char* functionName, int (*func)(lua_State*), lua_State* L)
{
    // It is assumed that the last element of 
    // the lua stack is the cubos table.

    lua_pushcfunction(L, func);
    lua_setfield(L, -2, functionName);
}

void cubos::bindings::lua::luaBindingsPlugin(Cubos& cubos)
{
    cubos.depends(settingsPlugin);

    cubos.uninitResource<State>();

    cubos.startupSystem("initialize lua bindings").before(settingsTag).call([&](Commands cmds) {
        lua_State* L = luaL_newstate();
        luaL_openlibs(L);

        // Set up cubos table.
        lua_newtable(L);
        lua_pushvalue(L, -1); // Save a copy of the table to be used later on.
        lua_setglobal(L, "cubos");

        // Save the pointer to the current cubos instance as cubos.instance.
        lua_pushlightuserdata(L, &cubos);
        lua_setfield(L, -2, "instance");

        // Add logging functions.
        addLuaFunction("info", logInfo, L);
        addLuaFunction("warn", logWarn, L);
        addLuaFunction("error", logError, L);
        addLuaFunction("critical", logCritical, L);

        // Remap print to cubos.log
        lua_pushcfunction(L, info);
        lua_setglobal(L, "print");
        
        cmds.emplaceResource<State>(L);
    });

    cubos.startupSystem("load lua scripts").after(settingsTag).call([](Settings& settings, State& state) {
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

            loadScripts(ScriptsMountPoint, state);
        }
    });
}
