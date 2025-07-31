#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/plugins/gravity.hpp>
#include <cubos/engine/physics/rigid_body_bundle.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/perspective.hpp>
#include <cubos/engine/render/defaults/plugin.hpp>
#include <cubos/engine/render/defaults/target.hpp>
#include <cubos/engine/render/lights/directional.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/transform/position.hpp>
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::engine;

static const Asset<VoxelGrid> CarAsset = AnyAsset("059c16e7-a439-44c7-9bdc-6e069dba0c80");
static const Asset<VoxelPalette> PaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc220");

struct MaxTime
{
    CUBOS_ANONYMOUS_REFLECT(MaxTime);

    float max = 1.0F;
    float current = 0.0F;
};

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    cubos.resource<MaxTime>();

    cubos.plugin(settingsPlugin);
    cubos.plugin(windowPlugin);
    cubos.plugin(transformPlugin);
    cubos.plugin(assetsPlugin);
    cubos.plugin(renderDefaultsPlugin);
    cubos.plugin(voxelsPlugin);
    cubos.plugin(fixedStepPlugin);
    cubos.plugin(collisionsPlugin);
    cubos.plugin(physicsPlugin);
    cubos.plugin(physicsSolverPlugin);
    cubos.plugin(gravityPlugin);

    cubos.startupSystem("configure Assets").before(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
    });

    cubos.startupSystem("create Camera").call([](Commands cmds) {
        auto targetEnt = cmds.create().add(RenderTargetDefaults{}).entity();
        cmds.create()
            .relatedTo(targetEnt, DrawsTo{})
            .add(Camera{.zNear = 0.1F, .zFar = 100.0F})
            .add(PerspectiveCamera{.fovY = 60.0F})
            .add(LocalToWorld{})
            .add(Position{{50.0F, 50.0F, 50.0F}})
            .add(Rotation::lookingAt({-1.0F, -1.0F, -1.0F}, glm::vec3{0.0F, 1.0F, 0.0F}));
    });

    cubos.startupSystem("create the sun").call([](Commands cmds) {
        cmds.create()
            .add(DirectionalLight{glm::vec3(1.0F), 1.0F})
            .add(Rotation{glm::quat(glm::vec3(glm::radians(45.0F), glm::radians(45.0F), 0))});
    });

    cubos.startupSystem("set the palette").call([](RenderPalette& palette) { palette.asset = PaletteAsset; });

    cubos.startupSystem("create a car")
        .after(settingsTag)
        .after(assetsBridgeTag)
        .call([](Commands cmds, const Assets& assets) {
            // Calculate the necessary offset to center the model on (0, 0, 0).
            auto car = assets.read(CarAsset);
            glm::vec3 offset = glm::vec3(car->size().x, 0.0F, car->size().z) / -2.0F;

            // Create the car entity
            cmds.create()
                .add(RenderVoxelGrid{CarAsset, offset})
                .add(RigidBodyBundle{.mass = 500.0F})
                .add(Position{{0.0F, 0.0F, 0.0F}})
                .add(LocalToWorld{});
        });

    cubos.system("push the car")
        .tagged(physicsApplyForcesTag)
        .call([](Query<Velocity&, Force&, Impulse&> query, MaxTime& time, const DeltaTime& deltaTime) {
            for (auto [velocity, force, impulse] : query)
            {
                if (time.current < time.max)
                {
                    if (time.current == 0.0F)
                    {
                        impulse.add(glm::vec3(0.0F, 5000.0F, 0.0F));
                    }
                    force.add(glm::vec3(0.0F, 0.0F, -5000.0F));
                    time.current += deltaTime.value();
                }
            }
        });

    cubos.run();
}
