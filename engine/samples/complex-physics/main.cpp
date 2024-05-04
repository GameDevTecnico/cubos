#include <glm/gtc/random.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <cubos/core/gl/debug.hpp>

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/fixed_substep/plugin.hpp>
#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/plugins/gravity.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/renderer/directional_light.hpp>
#include <cubos/engine/renderer/environment.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/screen_picker/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/transform/position.hpp>
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::engine;

static const Asset<Scene> RedCubeSceneAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc221");
static const Asset<Scene> WhiteCubeSceneAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc222");
static const Asset<VoxelPalette> PaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc220");

static const int NUM_WALLS = 3;
static const int WALL_WIDTH = 11;
static const int WALL_HEIGHT = 5;
static const bool RANDOM_DIRECTIOn = true;
static const glm::vec3 AIM_POINT = glm::vec3(0.0F, 3.0F, 0.0F);

struct TimeBetweenShoots
{
    float max = 3.0F;
    float current = 0.0F;
};

glm::vec3 randomPosition()
{
    glm::vec3 position = glm::sphericalRand(12.0F);
    position.y = glm::abs(position.y) + 1.0F;
    return position;
}

glm::vec3 calculateDirection(glm::vec3 position)
{
    return glm::normalize(AIM_POINT - position);
}

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    cubos.resource<TimeBetweenShoots>();

    cubos.plugin(settingsPlugin);
    cubos.plugin(windowPlugin);
    cubos.plugin(transformPlugin);
    cubos.plugin(assetsPlugin);
    cubos.plugin(screenPickerPlugin);
    cubos.plugin(rendererPlugin);
    cubos.plugin(voxelsPlugin);
    cubos.plugin(fixedStepPlugin);
    cubos.plugin(collisionsPlugin);
    cubos.plugin(fixedSubstepPlugin);
    cubos.plugin(physicsPlugin);
    cubos.plugin(solverPlugin);
    cubos.plugin(gravityPlugin);
    cubos.plugin(gizmosPlugin);
    cubos.plugin(scenePlugin);

    cubos.startupSystem("configure Assets").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    });

    cubos.startupSystem("create Camera").call([](Commands cmds, ActiveCameras& activeCameras) {
        activeCameras.entities[0] = cmds.create()
                                        .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 1000.0F})
                                        .add(Position{{20.0F, 20.0F, 20.0F}})
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
        renderer->setPalette(*assets.read<VoxelPalette>(PaletteAsset));
    });

    cubos.startupSystem("set environment").call([](RendererEnvironment& environment) {
        environment.ambient = {0.4F, 0.4F, 0.4F};
        environment.skyGradient[0] = {0.6F, 1.0F, 0.8F};
        environment.skyGradient[1] = {0.25F, 0.65F, 1.0F};
    });

    cubos.startupSystem("load and spawn the scene").tagged(assetsTag).call([](Commands cmds, const Assets& assets) {
        // Spawn floor collider
        cmds.create()
            .add(Collider{})
            .add(BoxCollisionShape{cubos::core::geom::Box{.halfSize = glm::vec3{20.0F, 0.5F, 20.0F}}})
            .add(LocalToWorld{})
            .add(Position{{0.0F, 0.0F, 0.0F}})
            .add(PreviousPosition{{0.0F, 0.0F, 0.0F}})
            .add(Velocity{.vec = {0.0F, 0.0F, 0.0F}})
            .add(Force{})
            .add(Impulse{})
            .add(Mass{.mass = 1.0F, .inverseMass = 0.0F})
            .add(AccumulatedCorrection{{0.0F, 0.0F, 0.0F}});

        auto redCube = assets.read(RedCubeSceneAsset);
        auto whiteCube = assets.read(WhiteCubeSceneAsset);

        // Spawn walls
        // Red cubes have 50 kg mass
        // White cubes have 10 kg mass
        bool red = true;
        glm::vec3 nextPosition = {-4.5F, 1.0F, 0.5F};
        for (int b = 0; b < NUM_WALLS; b++)
        {
            nextPosition.x = -4.5F;
            for (int w = 0; w < WALL_WIDTH; w++)
            {
                nextPosition.y = 1.0F;
                for (int h = 0; h < WALL_HEIGHT; h++)
                {
                    auto builder = cmds.spawn(red ? redCube->blueprint : whiteCube->blueprint);
                    builder.add("cube", Position{.vec = nextPosition});
                    nextPosition.y += 1.0F;
                    red = !red;
                }
                nextPosition.x += 1.0F;
            }
            nextPosition.z += 1.0F;
        }
    });

    cubos.system("shoot cube")
        .tagged(physicsApplyForcesTag)
        .call([](Commands cmds, const DeltaTime& dt, const Assets& assets, TimeBetweenShoots& time) {
            time.current += dt.value();

            auto cube = assets.read(WhiteCubeSceneAsset);
            // shoot in regular intervals
            if (time.current >= time.max)
            {
                // create cube in a position
                auto builder = cmds.spawn(cube->blueprint);
                glm::vec3 position = RANDOM_DIRECTIOn ? randomPosition() : glm::vec3{0.0F, 3.0F, -7.0F};
                builder.add("cube", Position{.vec = position});

                // push cube in direction to the center
                glm::vec3 impulseDirection = calculateDirection(position);
                float impulseStrength = 250.0F;
                auto impulse = Impulse{};
                impulse.add(impulseDirection * impulseStrength);
                builder.add("cube", impulse);

                // rotate cube according to impulse direction
                builder.add("cube", Rotation::lookingAt(impulseDirection));

                time.current = 0.0F;
            }
        });

    /*
    cubos.startupSystem("load and set the Input Bindings")
        .tagged("cubos.assets")
        .call([](const Assets& assets, Input& input) {
            input.bind(*assets.read<InputBindings>(EditorBindingsAsset), 0);
            input.bind(*assets.read<InputBindings>(Player1BindingsAsset), 1);
            input.bind(*assets.read<InputBindings>(Player2BindingsAsset), 2);
        });
    */

    cubos.system("draw gizmos").call([](Gizmos& gizmos) {
        // Draw Ground
        gizmos.color({0.2F, 0.2F, 1.0F});
        gizmos.drawWireBox("wire box", {-20.0, -0.5, -20.0}, {20.0, 0.5, 20.0}, 0, Gizmos::Space::World);
    });

    cubos.run();
}
