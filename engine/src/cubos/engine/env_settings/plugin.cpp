#include <cubos/core/settings.hpp>

#include <cubos/engine/env_settings/plugin.hpp>

using cubos::core::Settings;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using namespace cubos::engine;

static void startup(Read<Arguments> args, Write<Settings> settings)
{
    for (const auto& arg : args->value)
    {
        auto token = arg.find('=');
        if (token == std::string::npos)
        {
            CUBOS_ERROR("Could not parse settings (missing '=' token)");
            return;
        }

        auto settingName = arg.substr(0, token);
        auto settingValue = arg.substr(token + 1);
        if (settingValue.empty())
        {
            CUBOS_ERROR("Setting <{}> must have value", settingName);
            return;
        }

        settings->setString(settingName, settingValue);
    }
}

void cubos::engine::envSettingsPlugin(Cubos& cubos)
{
    cubos.startupSystem(startup).tagged("cubos.settings").tagged("cubos.settings.env");
}
