#include <glm/gtc/random.hpp>

#include <cubos/core/geom/box.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/collisions/shapes/voxel.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/gizmos/target.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/perspective.hpp>
#include <cubos/engine/render/defaults/plugin.hpp>
#include <cubos/engine/render/defaults/target.hpp>
#include <cubos/engine/render/picker/plugin.hpp>
#include <cubos/engine/render/tone_mapping/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::geom::Box;
using cubos::core::io::Key;
using cubos::core::io::Modifiers;
using cubos::core::io::MouseButton;

using namespace cubos::engine;

static CUBOS_DEFINE_TAG(collisionsSampleUpdated);

struct State
{
    CUBOS_ANONYMOUS_REFLECT(State);

    bool collided = false;

    Entity a;
    Entity b;

    glm::vec3 aRotationAxis;
    glm::vec3 bRotationAxis;
};

/// [Get handles to assets]
static const Asset<VoxelGrid> CarAsset = AnyAsset("059c16e7-a439-44c7-9bdc-6e069dba0c75");
static const Asset<VoxelPalette> PaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc215");
/// [Get handles to assets]

int main()
{
    auto cubos = Cubos();

    cubos.plugin(defaultsPlugin);
    cubos.tag(gizmosDrawTag).after(toneMappingTag);

    cubos.resource<State>();

    cubos.startupSystem("setup camera").call([](Commands commands) {
        auto targetEnt = commands.create().add(RenderTargetDefaults{}).add(GizmosTarget{}).entity();
        commands.create()
            .relatedTo(targetEnt, DrawsTo{})
            .add(Camera{.zNear = 0.1F, .zFar = 100.0F})
            .add(PerspectiveCamera{.fovY = 60.0F})
            .add(LocalToWorld{})
            .add(Position{{-35.0F, 1.5F, 0.0F}})
            .add(Rotation::lookingAt({3.0F, -1.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F}));
    });

    cubos.startupSystem("configure Assets").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    });

    /// [Set palette]
    cubos.startupSystem("set palette").call([](RenderPalette& palette) { palette.asset = PaletteAsset; });
    /// [Set palette]

    cubos.startupSystem("create colliders").tagged(assetsTag).call([](State& state, Commands commands, Assets& assets) {
        auto car = assets.read(CarAsset);
        glm::vec3 offset = glm::vec3(car->size().x, car->size().y, car->size().z) / -2.0F;
        state.a = commands.create()
                      .add(Collider{})
                      .add(RenderVoxelGrid{CarAsset, offset})
                      .add(VoxelCollisionShape(CarAsset))
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, -30.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 500.0F, .velocity = {0.0F, 0.0F, 1.0F}})
                      .entity();
        state.aRotationAxis = glm::sphericalRand(1.0F);

        state.b = commands.create()
                      .add(Collider{})
                      .add(RenderVoxelGrid{CarAsset, offset})
                      .add(VoxelCollisionShape(CarAsset))
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, 10.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 500.0F, .velocity = {0.0F, 0.0F, -1.0F}})
                      .entity();
        state.bRotationAxis = glm::sphericalRand(1.0F);
    });

    cubos.system("move colliders")
        .before(transformUpdateTag)
        .call([](State& state, Query<Position&, Rotation&, Velocity&> query) {
            auto [aPos, aRot, aVel] = *query.at(state.a);
            auto [bPos, bRot, bVel] = *query.at(state.b);

            aRot.quat = glm::rotate(aRot.quat, 0.001F, state.aRotationAxis);
            aVel.vec += glm::vec3{0.0F, 0.0F, 0.01F};

            bRot.quat = glm::rotate(bRot.quat, 0.001F, state.bRotationAxis);
            bVel.vec -= glm::vec3{0.0F, 0.0F, 0.01F};
        });

    cubos.tag(collisionsSampleUpdated);

    cubos.system("render voxel")
        .after(collisionsSampleUpdated)
        .call([](Gizmos& gizmos, Query<const LocalToWorld&, const Collider&, const VoxelCollisionShape&> query) {
            for (auto [localToWorld, collider, shape] : query)
            {
                for (const auto box : shape.getBoxes())
                {
                    // Get the current position from the localToWorld matrix
                    glm::mat4 pos = localToWorld.mat; // Store the matrix

                    // Create a translation matrix for the shift
                    glm::mat4 shiftMatrix = glm::translate(glm::mat4(1.0F), -box.shift);

                    // Combine the matrices (note: order matters)
                    pos = pos * shiftMatrix;
                    auto size = box.box.halfSize * 2.0F;
                    glm::mat4 transform = glm::scale(pos * collider.transform, size);
                    gizmos.drawWireBox("subboxes", transform);
                }
            }
        });

    cubos.system("render")
        .after(collisionsSampleUpdated)
        .call([](Gizmos& gizmos, Query<const LocalToWorld&, const Collider&> query) {
            for (auto [localToWorld, collider] : query)
            {
                auto size = collider.localAABB.box().halfSize * 2.0F;
                glm::mat4 transform = glm::scale(localToWorld.mat * collider.transform, size);
                gizmos.color({1.0F, 1.0F, 1.0F});
                gizmos.drawWireBox("local AABB", transform);

                gizmos.color({1.0F, 0.0F, 0.0F});
                gizmos.drawWireBox("world AABB", collider.worldAABB.min(), collider.worldAABB.max());
            }
        });

    cubos.run();
    return 0;
}
