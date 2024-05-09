#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/child_of.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/ui/canvas/canvas.hpp>
#include <cubos/engine/ui/canvas/element.hpp>
#include <cubos/engine/ui/canvas/horizontal_stretch.hpp>
#include <cubos/engine/ui/canvas/plugin.hpp>
#include <cubos/engine/ui/canvas/vertical_stretch.hpp>
#include <cubos/engine/ui/color_rect/color_rect.hpp>
#include <cubos/engine/ui/color_rect/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::engine;

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    cubos.plugin(settingsPlugin);
    cubos.plugin(windowPlugin);
    cubos.plugin(assetsPlugin);
    cubos.plugin(renderTargetPlugin);
    cubos.plugin(shaderPlugin);
    cubos.plugin(transformPlugin);
    cubos.plugin(uiCanvasPlugin);
    cubos.plugin(colorRectPlugin);

    cubos.startupSystem("create ui elements").after(windowInitTag).call([](Commands commands) {
        /// [Set up Canvas]
        auto canvas = commands.create().add(UICanvas{}).add(RenderTarget{}).entity();
        /// [Set up Canvas]
        /// [Set up Background]
        auto elementBg = commands.create()
                             .add(UIElement{})
                             .add(UIHorizontalStretch{20, 20})
                             .add(UIVerticalStretch{20, 20})
                             .add(UIColorRect{{1, 1, 1, 1}})
                             .entity();
        commands.relate(elementBg, canvas, ChildOf{});
        /// [Set up Background]

        /// [Set up Panel]
        auto elementPanel = commands.create()
                                .add(UIElement{{0, -50}, {200, 600}, {1, 0.5F}, {1, 0.5F}})
                                .add(UIColorRect{{1, 0, 0, 1}})
                                .entity();
        commands.relate(elementPanel, elementBg, ChildOf{});
        /// [Set up Panel]
    });

    cubos.run();
}
