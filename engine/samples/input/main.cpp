#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::io::Window;

using namespace cubos::engine;

/// [Setting the Bindings]
static const Asset<InputBindings> BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d7842c3");
/// [Setting the Bindings]

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

/// [Showcase Action Press]
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

static void showcaseJustXZ(const Input& input, bool& explained)
{
    if (!explained)
    {
        CUBOS_WARN("This showcase will print `justPressed X or Z` when either X or Z is justPressed and `justReleased "
                   "X or Z when either X or Z is justReleased`. Press Enter to advance to the "
                   "next showcase.");
        explained = true;
    }

    if (input.justPressed("x-or-z"))
    {
        CUBOS_INFO("justPressed X or Z");
    }
    if (input.justReleased("x-or-z"))
    {
        CUBOS_INFO("justReleased X or Z");
    }
}

/// [Showcase Action Press]

/// [Showcase Axis]
static void showcaseAxis(const Input& input, bool& explained)
{
    if (!explained)
    {
        CUBOS_WARN("Axis are supported. This showcase will print the value of the `horizontal` and `vertical` axis "
                   "when their values is different to 0. Use the arrows and WASD to move the axis. Press Enter to "
                   "advance to the next showcase.");
        explained = true;
    }

    if (input.axis("horizontal") != 0.0F || input.axis("vertical") != 0.0F)
    {
        CUBOS_INFO("horizontal: {}, vertical: {}", input.axis("horizontal"), input.axis("vertical"));
    }
}
/// [Showcase Axis]

/// [Showcase Unbound]
static void showcaseUnbound(const Window& window, bool& explained)
{
    if (!explained)
    {
        CUBOS_WARN("Direct access is supported. This showcase will print `Unbound` when Y is pressed. Note "
                   "that Y is not bound in sample.bind. Press Enter to advance to the next showcase.");
        explained = true;
    }

    /// When no action is bound to a key, its state can still be accessed directly through the Window.
    if (window->pressed(Input::Key::Y))
    {
        CUBOS_INFO("Unbound");
    }
}
/// [Showcase Unbound]

/// [Showcase Mouse Buttons]
static void showcaseMouseButtons(const Input& input, bool& explained)
{
    if (!explained)
    {
        CUBOS_WARN("This showcase will print the name of a mouse button when it is pressed. Press Enter to advance to "
                   "the next showcase.");
        explained = true;
    }

    if (input.pressed("left-mb"))
    {
        CUBOS_INFO("Left");
    }
    if (input.pressed("right-mb"))
    {
        CUBOS_INFO("Right");
    }
    if (input.pressed("middle-mb"))
    {
        CUBOS_INFO("Middle");
    }
    if (input.pressed("extra-mb"))
    {
        CUBOS_INFO("Extra1 or Extra2");
    }
}
/// [Showcase Mouse Buttons]

int main()
{
    auto cubos = Cubos();

    /// [Adding the plugin]
    cubos.plugin(settingsPlugin);
    cubos.plugin(windowPlugin);
    cubos.plugin(assetsPlugin);
    cubos.plugin(inputPlugin);
    /// [Adding the plugin]

    cubos.resource<State>();

    cubos.startupSystem("configure Assets").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    });

    /// [Loading the bindings]
    cubos.startupSystem("load and set the Input Bindings")
        .tagged(assetsTag)
        .call([](const Assets& assets, Input& input) {
            auto bindings = assets.read<InputBindings>(BindingsAsset);
            input.bind(*bindings);
            CUBOS_INFO("Loaded bindings: {}", input.bindings().at(0));
        });
    /// [Loading the bindings]

    /// [Checking Type of Press]
    cubos.system("detect input")
        .after(inputUpdateTag)
        .call([](const Input& input, const Window& window, State& state, ShouldQuit& shouldQuit) {
            // FIXME: This is an hack to have one-shot actions while we don't have input events.
            if (input.pressed("next-showcase"))
            {
                state.nextPressed = true;
            }
            else if (state.nextPressed)
            {
                state.nextPressed = false;
                state.explained = false;
                state.showcase++;
            }
            /// [Checking Type of Press]

            switch (state.showcase)
            {
            case 0:
                return explain(state.explained);
            case 1:
                return showcaseXZ(input, state.explained);
            case 2:
                return showcaseJustXZ(input, state.explained);
            case 3:
                return showcaseAxis(input, state.explained);
            case 4:
                return showcaseUnbound(window, state.explained);
            case 5:
                return showcaseMouseButtons(input, state.explained);
            default:
                shouldQuit.value = true;
            }
        });

    cubos.run();
    return 0;
}
