#include <cubos/bindings/lua/plugin.hpp>
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

void cubos::bindings::lua::luaBindingsPlugin(Cubos& cubos)
{
    cubos.depends(settingsPlugin);

    cubos.uninitResource<State>();

    cubos.startupSystem("initialize lua bindings").before(settingsTag).call([](Commands cmds) {
        lua_State* state = luaL_newstate();
        luaL_openlibs(state);
        // TODO: add custom cubos functions here
        cmds.emplaceResource<State>(state);
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
