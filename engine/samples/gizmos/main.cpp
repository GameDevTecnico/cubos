#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/gizmos/target.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/perspective_camera.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/picker/picker.hpp>
#include <cubos/engine/render/picker/plugin.hpp>
#include <cubos/engine/render/split_screen/plugin.hpp>
#include <cubos/engine/render/split_screen/split_screen.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::engine;

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.plugin(settingsPlugin);
    cubos.plugin(windowPlugin);
    cubos.plugin(transformPlugin);
    cubos.plugin(renderTargetPlugin);
    cubos.plugin(renderPickerPlugin);
    cubos.plugin(cameraPlugin);
    cubos.plugin(splitScreenPlugin);

    /// [Adding plugin]
    cubos.plugin(gizmosPlugin);
    /// [Adding plugin]

    cubos.startupSystem("disable Assets IO").tagged(settingsTag).call([](Settings& settings) {
        // We don't load assets in this sample and we don't even have an assets folder, so we should
        // disable assets IO.
        settings.setBool("assets.io.enabled", false);
    });

    cubos.startupSystem("setup render target and cameras").call([](Commands cmds) {
        auto target =
            cmds.create().add(RenderTarget{}).add(RenderPicker{}).add(GizmosTarget{}).add(SplitScreen{}).entity();

        cmds.create()
            .relatedTo(target, DrawsTo{})
            .add(PerspectiveCamera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(LocalToWorld{})
            .add(Position{{5.0F, 5.0F, 0.0F}})
            .add(Rotation{glm::quatLookAt(glm::vec3{-1.0F, -1.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F})});

        cmds.create()
            .relatedTo(target, DrawsTo{})
            .add(PerspectiveCamera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(LocalToWorld{})
            .add(Position{{-25.0F, 25.0F, 0.0F}})
            .add(Rotation{glm::quatLookAt(glm::vec3{1.0F, -1.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F})});

        cmds.create()
            .relatedTo(target, DrawsTo{})
            .add(PerspectiveCamera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(LocalToWorld{})
            .add(Position{{-25.0F, -25.0F, 0.0F}})
            .add(Rotation{glm::quatLookAt(glm::vec3{1.0F, 1.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();

        cmds.create()
            .relatedTo(target, DrawsTo{})
            .add(PerspectiveCamera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(LocalToWorld{})
            .add(Position{{25.0F, -25.0F, 0.0F}})
            .add(Rotation{glm::quatLookAt(glm::vec3{-1.0F, 1.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();
    });

    /// [Start Up System]
    cubos.startupSystem("draw line at startup").after(gizmosInitTag).call([](Gizmos& gizmos) {
        gizmos.color({1, 0, 1});
        gizmos.drawArrow("arrow", {0.6F, 0.6F, 0.0F}, {-0.1F, -0.1F, 0.0F}, 0.003F, 0.009F, 0.7F, 10.0F,
                         Gizmos::Space::Screen);
    });
    /// [Start Up System]

    /// [System]
    cubos.system("draw gizmos").call([](Gizmos& gizmos) {
        /// [Lines]
        gizmos.color({1.0F, 1.0F, 1.0F});
        gizmos.drawLine("separator line", {1.0F, 0.5F, 0.5F}, {0.0F, 0.5F, 0.5F}, 0, Gizmos::Space::Screen);
        gizmos.drawLine("separator line", {0.5F, 1.0F, 0.5F}, {0.5F, 0.0F, 0.5F}, 0, Gizmos::Space::Screen);
        /// [Lines]

        /// [WireBox]
        gizmos.color({1.0F, 0.5F, 1.0F});
        gizmos.drawBox("box", {0.4, 0.4, 0}, {0.55, 0.55, 0}, 0, Gizmos::Space::View);
        /// [WireBox]

        /// [Box]
        gizmos.color({0.2F, 0.2F, 1.0F});
        gizmos.drawWireBox("wire box", {-5, -5, -5}, {-7, -7, -7}, 0, Gizmos::Space::World);
        /// [Box]

        ///[Cut Cone]
        if (gizmos.hovered("cut cone"))
        {
            gizmos.color({0.25F, 0.15F, 0.5F});
        }
        else if (gizmos.pressed("cut cone"))
        {
            gizmos.color({0.5F, 0.3F, 1});
        }
        else
        {
            gizmos.color({0.1F, 0.05F, 0.25F});
        }

        gizmos.drawCutCone("cut cone", {0.7F, 0.7F, 0.7F}, 5.0F, {-3, -3, -3}, 3.0F, 0, Gizmos::Space::World);
        ///[Cut Cone]
    });
    /// [System]

    cubos.run();
}
