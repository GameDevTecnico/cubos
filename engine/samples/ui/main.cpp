#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/image/plugin.hpp>
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
#include <cubos/engine/ui/canvas/native_aspect_ratio.hpp>
#include <cubos/engine/ui/canvas/plugin.hpp>
#include <cubos/engine/ui/canvas/vertical_stretch.hpp>
#include <cubos/engine/ui/color_rect/color_rect.hpp>
#include <cubos/engine/ui/color_rect/plugin.hpp>
#include <cubos/engine/ui/image/image.hpp>
#include <cubos/engine/ui/image/plugin.hpp>
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
    cubos.plugin(imagePlugin);
    cubos.plugin(uiImagePlugin);
    cubos.plugin(colorRectPlugin);

    cubos.startupSystem("configure Assets").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    });

    cubos.startupSystem("create ui elements").after(windowInitTag).after(settingsTag).call([](Commands commands) {
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
        auto elementPanel =
            commands.create()
                .add(UIElement{.offset = {-50, 0}, .size = {200, 600}, .pivot = {1, 0.5F}, .anchor = {1, 0.5F}})
                .add(UIColorRect{{1, 0, 0, 1}})
                .entity();
        commands.relate(elementPanel, elementBg, ChildOf{});
        /// [Set up Panel]

        /// [Set up Logo]
        auto logo = commands.create()
                        .add(UIElement{.offset = {50, -50}, .size = {200, 200}, .pivot = {0, 1}, .anchor = {0, 1}})
                        .add(UIImage{AnyAsset("50423317-a543-4614-9f4e-c2df975f5c0d")})
                        .entity();
        commands.relate(logo, elementBg, ChildOf{});
        /// [Set up Logo]

        /// [Set up Long Logo]
        auto longLogo = commands.create()
                            .add(UIElement{.offset = {50, 50}, .size = {400, 400}, .pivot = {0, 0}, .anchor = {0, 0}})
                            .add(UIImage{AnyAsset("6c24c031-2eac-47c9-be30-485238c3e355")})
                            .add(UINativeAspectRatio{})
                            .entity();
        commands.relate(longLogo, elementBg, ChildOf{});
        /// [Set up Long Logo]
    });

    cubos.run();
}
