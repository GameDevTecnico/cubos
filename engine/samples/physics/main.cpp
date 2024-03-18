#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/plugins/gravity.hpp>
#include <cubos/engine/physics/solver/solver.hpp>
#include <cubos/engine/renderer/directional_light.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/screen_picker/plugin.hpp>
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
    cubos.plugin(screenPickerPlugin);
    cubos.plugin(rendererPlugin);
    cubos.plugin(voxelsPlugin);
    cubos.plugin(fixedStepPlugin);
    cubos.plugin(collisionsPlugin);
    cubos.plugin(physicsPlugin);
    cubos.plugin(solverPlugin);
    cubos.plugin(gravityPlugin);

    cubos.startupSystem("configure Assets").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    });

    cubos.startupSystem("create Camera").call([](Commands cmds, ActiveCameras& activeCameras) {
        activeCameras.entities[0] = cmds.create()
                                        .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 1000.0F})
                                        .add(Position{{50.0F, 50.0F, 50.0F}})
                                        .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{-1.0F, -1.0F, -1.0F}),
                                                                      glm::vec3{0.0F, 1.0F, 0.0F})})
                                        .entity();
    });

    cubos.startupSystem("create the sun").call([](Commands cmds) {
        cmds.create()
            .add(DirectionalLight{glm::vec3(1.0F), 1.0F})
            .add(Rotation{glm::quat(glm::vec3(glm::radians(45.0F), glm::radians(45.0F), 0))});
    });

    cubos.startupSystem("load the palette").after(rendererInitTag).call([](const Assets& assets, Renderer& renderer) {
        auto palette = assets.read(PaletteAsset);
        renderer->setPalette(*palette);
    });

    cubos.startupSystem("create a car")
        .after(settingsTag)
        .after(assetsBridgeTag)
        .call([](Commands cmds, const Assets& assets) {
            // Calculate the necessary offset to center the model on (0, 0, 0).
            auto car = assets.read(CarAsset);
            glm::vec3 offset = glm::vec3(car->size().x, 0.0F, car->size().z) / -2.0F;

            // Create the car entity
            cmds.create()
                .add(RenderableGrid{CarAsset, offset})
                .add(Position{{0.0F, 0.0F, 0.0F}})
                .add(PreviousPosition{{0.0F, 0.0F, 0.0F}})
                .add(Velocity{.vec = {0.0F, 0.0F, 0.0F}})
                .add(Force{})
                .add(Impulse{})
                .add(Mass{.mass = 500.0F, .inverseMass = 1.0F / 500.0F})
                .add(AccumulatedCorrection{{0.0F, 0.0F, 0.0F}})
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
                    time.current += deltaTime.value;
                }
            }
        });

    cubos.run();
}
