#include <cubos/core/data/debug_serializer.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/input/plugin.hpp>

using cubos::core::Settings;
using cubos::core::data::Debug;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using namespace cubos::engine;

static const Asset<InputBindings> bindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d7842c3");
int showcase = 0;
bool nextPressed = false;
bool explained = false;

static void explain()
{
    if (!explained)
    {
        CUBOS_WARN("This sample will present multiple showcases of the input system. Press Enter to advance to the "
                   "next showcase.");
        explained = true;
    }
}

static void showcase_x_or_z(Read<Input> input)
{
    if (!explained)
    {
        CUBOS_WARN("This showcase will print `X or Z` when either X or Z is pressed. Press Enter to advance to the "
                   "next showcase.");
        explained = true;
    }

    if (input->pressed("x-or-z"))
    {
        CUBOS_INFO("X or Z");
    }
}

static void showcase_modifiers(Read<Input> input)
{
    if (!explained)
    {
        CUBOS_WARN("Modifiers are supported. This showcase will print `Shift` when Shift+Space is pressed, `Alt` when "
                   "Alt+Space is pressed, `Ctrl` when Ctrl+Space is pressed and `System` when System+Space is pressed. "
                   "Press Enter to advance to the next showcase.");
        explained = true;
    }

    if (input->pressed("shift-space"))
    {
        CUBOS_INFO("Shift");
    }

    if (input->pressed("alt-space"))
    {
        CUBOS_INFO("Alt");
    }

    if (input->pressed("ctrl-space"))
    {
        CUBOS_INFO("Ctrl");
    }

    if (input->pressed("system-space"))
    {
        CUBOS_INFO("System");
    }
}

static void showcase_multiple_modifiers(Read<Input> input)
{
    if (!explained)
    {
        CUBOS_WARN("Multiple modifiers are supported. This showcase will print `Ctrl Shift` when Ctrl+Shift+Space is "
                   "pressed. Press Enter to advance to the next showcase.");
        explained = true;
    }

    if (input->pressed("ctrl-shift-space"))
    {
        CUBOS_INFO("Ctrl Shift");
    }
}

static void showcase_modifier_keys(Read<Input> input)
{
    if (!explained)
    {
        CUBOS_WARN("Modifier keys are supported. This showcase will print `Ctrl` when Ctrl is pressed and `Ctrl Shift` "
                   "when Ctrl+Shift is pressed. Press Enter to advance to the next showcase.");
        explained = true;
    }

    if (input->pressed("ctrl"))
    {
        CUBOS_INFO("Ctrl");
    }

    if (input->pressed("ctrl-shift"))
    {
        CUBOS_INFO("Ctrl Shift");
    }
}

static void showcase_axis(Read<Input> input)
{
    if (!explained)
    {
        CUBOS_WARN("Axis are supported. This showcase will print the value of the `horizontal` and `vertical` axis "
                   "when their values is different to 0. Use the arrows and WASD to move the axis. Press Enter to "
                   "advance to the next showcase.");
        explained = true;
    }

    if (input->axis("horizontal") != 0.0f || input->axis("vertical") != 0.0f)
    {
        CUBOS_INFO("horizontal: {}, vertical: {}", input->axis("horizontal"), input->axis("vertical"));
    }
}

static void showcase_modifier_axis(Read<Input> input)
{
    if (!explained)
    {
        CUBOS_WARN("Modifiers are supported with axis. This showcase will print the value of the `shift-vertical` axis "
                   "when its value is different to 0. Use Shift+arrows and Shift+WASD to move the axis. Press Enter to "
                   "advance to the next showcase.");
        explained = true;
    }

    if (input->axis("shift-vertical") != 0.0f)
    {
        CUBOS_INFO("shift-vertical: {}", input->axis("shift-vertical"));
    }
}

static void update(Read<Input> input, Write<ShouldQuit> shouldQuit)
{
    // FIXME: This is an hack to have one-shot actions while we don't have input events.
    if (input->pressed("next_showcase"))
    {
        nextPressed = true;
    }
    else if (nextPressed)
    {
        nextPressed = false;
        explained = false;
        showcase++;
    }

    switch (showcase)
    {
    case 0:
        return explain();
    case 1:
        return showcase_x_or_z(input);
    case 2:
        return showcase_modifiers(input);
    case 3:
        return showcase_multiple_modifiers(input);
    case 4:
        return showcase_modifier_keys(input);
    case 5:
        return showcase_axis(input);
    case 6:
        return showcase_modifier_axis(input);
    default:
        shouldQuit->value = true;
    }
}

static void config(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

static void init(Read<Assets> assets, Write<Input> input)
{
    auto bindings = assets->read<InputBindings>(bindingsAsset);
    input->bind(*bindings);
    CUBOS_INFO("Loaded bindings: {}", Debug(input->bindings().at(0)));
}

int main()
{
    auto cubos = Cubos();
    cubos.addPlugin(inputPlugin);
    cubos.startupSystem(config).tagged("cubos.settings");
    cubos.startupSystem(init).tagged("cubos.assets");
    cubos.system(update).afterTag("cubos.input.update");
    cubos.run();
    return 0;
}
