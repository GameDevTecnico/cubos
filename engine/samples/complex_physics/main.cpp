#include <glm/gtc/random.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/collisions/collision_layers.hpp>
#include <cubos/engine/collisions/collision_mask.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/interpolation/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/plugins/gravity.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/perspective.hpp>
#include <cubos/engine/render/defaults/plugin.hpp>
#include <cubos/engine/render/defaults/target.hpp>
#include <cubos/engine/render/lights/directional.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
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

static const int NumWalls = 3;
static const int WallWidth = 11;
static const int WallHeight = 5;
static const bool RandomDirection = true;
static const glm::vec3 AimPoint = glm::vec3(0.0F, 3.0F, 0.0F);

struct TimeBetweenShoots
{
    CUBOS_ANONYMOUS_REFLECT(TimeBetweenShoots);

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
    return glm::normalize(AimPoint - position);
}

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    cubos.resource<TimeBetweenShoots>();

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
    // cubos.plugin(gizmosPlugin);
    cubos.plugin(scenePlugin);

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
            .add(Position{{20.0F, 20.0F, 20.0F}})
            .add(Rotation::lookingAt({-1.0F, -1.0F, -1.0F}, glm::vec3{0.0F, 1.0F, 0.0F}));
    });

    cubos.startupSystem("create the sun").call([](Commands cmds) {
        cmds.create()
            .add(DirectionalLight{glm::vec3(1.0F), 1.0F})
            .add(Rotation{glm::quat(glm::vec3(glm::radians(45.0F), glm::radians(45.0F), 0))});
    });

    cubos.startupSystem("set the palette").call([](RenderPalette& palette) { palette.asset = PaletteAsset; });

    cubos.startupSystem("set environment").call([](RenderEnvironment& environment) {
        environment.ambient = {0.4F, 0.4F, 0.4F};
        environment.skyGradient[0] = {0.6F, 1.0F, 0.8F};
        environment.skyGradient[1] = {0.25F, 0.65F, 1.0F};
    });

    cubos.startupSystem("load and spawn the scene").tagged(assetsTag).call([](Commands cmds, const Assets& assets) {
        // Spawn floor collider
        cmds.create()
            .add(BoxCollisionShape{cubos::core::geom::Box{.halfSize = glm::vec3{20.0F, 0.5F, 20.0F}}})
            .add(CollisionLayers{})
            .add(CollisionMask{})
            .add(LocalToWorld{})
            .add(Position{{0.0F, 0.0F, 0.0F}})
            .add(Rotation{})
            .add(Velocity{.vec = {0.0F, 0.0F, 0.0F}})
            .add(AngularVelocity{})
            .add(Force{})
            .add(Torque{})
            .add(Impulse{})
            .add(AngularImpulse{})
            .add(Mass{.mass = 1.0F, .inverseMass = 0.0F})
            .add(CenterOfMass{})
            .add(AccumulatedCorrection{{0.0F, 0.0F, 0.0F}})
            .add(Inertia{.inertia = glm::mat3(0.0F), .inverseInertia = glm::mat3(0.0F), .autoUpdate = true})
            .add(PhysicsMaterial{.friction = 0.1F});

        auto redCube = assets.read(RedCubeSceneAsset);
        auto whiteCube = assets.read(WhiteCubeSceneAsset);

        // Spawn walls
        // Red cubes have 50 kg mass
        // White cubes have 10 kg mass
        bool red = true;
        glm::vec3 nextPosition = {-4.5F, 1.0F, 0.5F};
        for (int b = 0; b < NumWalls; b++)
        {
            nextPosition.x = -4.5F;
            for (int w = 0; w < WallWidth; w++)
            {
                nextPosition.y = 1.0F;
                for (int h = 0; h < WallHeight; h++)
                {
                    auto builder = cmds.spawn(red ? redCube->blueprint() : whiteCube->blueprint());
                    builder.add(Position{.vec = nextPosition});
                    builder.add(Interpolated{});
                    nextPosition.y += 1.0F;
                    red = !red;
                }
                nextPosition.x += 1.0F;
            }
            nextPosition.z += 1.0F;
        }
    });

    cubos.system("shoot cube")
        .before(transformUpdateTag)
        .call([](Commands cmds, const DeltaTime& dt, const Assets& assets, TimeBetweenShoots& time) {
            time.current += dt.value();

            auto cube = assets.read(WhiteCubeSceneAsset);
            // shoot in regular intervals
            if (time.current >= time.max)
            {
                // create cube in a position
                auto builder = cmds.spawn(cube->blueprint());
                glm::vec3 position = RandomDirection ? randomPosition() : glm::vec3{0.0F, 3.0F, -7.0F};
                builder.add(Position{.vec = position});
                builder.add(Interpolated{});

                // push cube in direction to the center
                glm::vec3 impulseDirection = calculateDirection(position);
                float impulseStrength = 250.0F;
                auto impulse = Impulse{};
                impulse.add(impulseDirection * impulseStrength);
                builder.add(impulse);

                // rotate cube according to impulse direction
                builder.add(Rotation::lookingAt(impulseDirection));

                time.current = 0.0F;
            }
        });

    /*cubos.system("draw gizmos").call([](Gizmos& gizmos) {
        // Draw Ground
        gizmos.color({0.2F, 0.2F, 1.0F});
        gizmos.drawWireBox("wire box", {-20.0, -0.5, -20.0}, {20.0, 0.5, 20.0}, 0, Gizmos::Space::World);
    });*/

    cubos.run();
}
