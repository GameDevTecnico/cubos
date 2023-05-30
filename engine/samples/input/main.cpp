#include <cubos/core/data/debug_serializer.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/input/plugin.hpp>

using cubos::core::Settings;
using cubos::core::data::Debug;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::io::Key;
using cubos::core::io::Modifiers;
using cubos::core::io::Window;
using namespace cubos::engine;

static const Asset<InputBindings> bindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d7842c3");

struct State
{
    int showcase = 0;
    bool nextPressed = false;
    bool explained = false;
};

static void explain(bool& explained)
{
    if (!explained)
    {
        CUBOS_WARN("This sample will present multiple showcases of the input system. Press Enter to advance to the "
                   "next showcase.");
        explained = true;
    }
}

static void showcaseXZ(const Input& input, bool& explained)
{
    if (!explained)
    {
        CUBOS_WARN("This showcase will print `X or Z` when either X or Z is pressed. Press Enter to advance to the "
                   "next showcase.");
        explained = true;
    }

    if (input.pressed("x-or-z"))
    {
        CUBOS_INFO("X or Z");
    }
}

static void showcaseModifiers(const Input& input, bool& explained)
{
    if (!explained)
    {
        CUBOS_WARN("Modifiers are supported. This showcase will print `Shift` when Shift+Space is pressed, `Alt` when "
                   "Alt+Space is pressed, `Ctrl` when Ctrl+Space is pressed and `System` when System+Space is pressed. "
                   "Press Enter to advance to the next showcase.");
        explained = true;
    }

    if (input.pressed("shift-space"))
    {
        CUBOS_INFO("Shift");
    }

    if (input.pressed("alt-space"))
    {
        CUBOS_INFO("Alt");
    }

    if (input.pressed("ctrl-space"))
    {
        CUBOS_INFO("Ctrl");
    }

    if (input.pressed("system-space"))
    {
        CUBOS_INFO("System");
    }
}

static void showcaseMultipleModifiers(const Input& input, bool& explained)
{
    if (!explained)
    {
        CUBOS_WARN("Multiple modifiers are supported. This showcase will print `Ctrl Shift` when Ctrl+Shift+Space is "
                   "pressed. Press Enter to advance to the next showcase.");
        explained = true;
    }

    if (input.pressed("ctrl-shift-space"))
    {
        CUBOS_INFO("Ctrl Shift");
    }
}

static void showcaseModifierKeys(const Input& input, bool& explained)
{
    if (!explained)
    {
        CUBOS_WARN("Modifier keys are supported. This showcase will print `Ctrl` when Ctrl is pressed and `Ctrl Shift` "
                   "when Ctrl+Shift is pressed. Press Enter to advance to the next showcase.");
        explained = true;
    }

    if (input.pressed("ctrl"))
    {
        CUBOS_INFO("Ctrl");
    }

    if (input.pressed("ctrl-shift"))
    {
        CUBOS_INFO("Ctrl Shift");
    }
}

static void showcaseAxis(const Input& input, bool& explained)
{
    if (!explained)
    {
        CUBOS_WARN("Axis are supported. This showcase will print the value of the `horizontal` and `vertical` axis "
                   "when their values is different to 0. Use the arrows and WASD to move the axis. Press Enter to "
                   "advance to the next showcase.");
        explained = true;
    }

    if (input.axis("horizontal") != 0.0f || input.axis("vertical") != 0.0f)
    {
        CUBOS_INFO("horizontal: {}, vertical: {}", input.axis("horizontal"), input.axis("vertical"));
    }
}

static void showcaseModifierAxis(const Input& input, bool& explained)
{
    if (!explained)
    {
        CUBOS_WARN("Modifiers are supported with axis. This showcase will print the value of the `shift-vertical` axis "
                   "when its value is different to 0. Use Shift+arrows and Shift+WASD to move the axis. Press Enter to "
                   "advance to the next showcase.");
        explained = true;
    }

    if (input.axis("shift-vertical") != 0.0f)
    {
        CUBOS_INFO("shift-vertical: {}", input.axis("shift-vertical"));
    }
}

static void showcaseUnbound(const Window& window, bool& explained)
{
    if (!explained)
    {
        CUBOS_WARN("Direct access is supported. This showcase will print `Unbound` when Ctrl+Shift+Y is pressed. Note "
                   "that Ctrl+Shift+Y is not bound in sample.bind. Press Enter to advance to the next showcase.");
        explained = true;
    }

    /// When no action is bound to a key, its state can still be accessed directly through the Window.
    if (window->keyPressed(Input::Key::Y, Modifiers::Shift | Input::Modifiers::Control))
    {
        CUBOS_INFO("Unbound");
    }
}

static void update(Read<Input> input, Read<Window> window, Write<State> state, Write<ShouldQuit> shouldQuit)
{
    // FIXME: This is an hack to have one-shot actions while we don't have input events.
    if (input->pressed("next_showcase"))
    {
        state->nextPressed = true;
    }
    else if (state->nextPressed)
    {
        state->nextPressed = false;
        state->explained = false;
        state->showcase++;
    }

    switch (state->showcase)
    {
    case 0:
        return explain(state->explained);
    case 1:
        return showcaseXZ(*input, state->explained);
    case 2:
        return showcaseModifiers(*input, state->explained);
    case 3:
        return showcaseMultipleModifiers(*input, state->explained);
    case 4:
        return showcaseModifierKeys(*input, state->explained);
    case 5:
        return showcaseAxis(*input, state->explained);
    case 6:
        return showcaseModifierAxis(*input, state->explained);
    case 7:
        return showcaseUnbound(*window, state->explained);
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

    cubos.addResource<State>();

    cubos.startupSystem(config).tagged("cubos.settings");
    cubos.startupSystem(init).tagged("cubos.assets");

    cubos.system(update).afterTag("cubos.input.update");

    cubos.run();
    return 0;
}
