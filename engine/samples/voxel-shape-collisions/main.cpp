#include <glm/gtc/random.hpp>

#include <cubos/core/geom/box.hpp>
#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/collisions/collider_aabb.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/collision_layers.hpp> //maybe put this in the collisions plugin
#include <cubos/engine/collisions/collision_mask.hpp>
#include <cubos/engine/collisions/contact_manifold.hpp>
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

struct DebugDraw
{
    CUBOS_ANONYMOUS_REFLECT(DebugDraw);

    bool normal = true;
    bool points = true;
    bool manifoldPolygon = true;
};

struct State
{
    CUBOS_ANONYMOUS_REFLECT(State);

    bool collided = false;

    Entity a;
    Entity b;

    glm::vec3 aRotationAxis;
    glm::vec3 bRotationAxis;
};

static const Asset<VoxelGrid> CarAsset = AnyAsset("059c16e7-a439-44c7-9bdc-6e069dba0c75");
static const Asset<VoxelPalette> PaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc215");

int main()
{
    auto cubos = Cubos();

    cubos.plugin(defaultsPlugin);
    cubos.tag(gizmosDrawTag).after(toneMappingTag);

    cubos.resource<State>();
    cubos.resource<DebugDraw>();

    cubos.startupSystem("setup camera").call([](Commands commands) {
        auto targetEnt = commands.create().add(RenderTargetDefaults{}).add(GizmosTarget{}).entity();
        commands.create()
            .relatedTo(targetEnt, DrawsTo{})
            .add(Camera{.zNear = 0.1F, .zFar = 100.0F})
            .add(PerspectiveCamera{.fovY = 60.0F})
            .add(LocalToWorld{})
            .add(Position{{-50.0F, 1.5F, 0.0F}})
            .add(Rotation::lookingAt({3.0F, -1.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F}));
    });

    cubos.startupSystem("configure Assets").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
    });

    cubos.startupSystem("set palette").call([](RenderPalette& palette) { palette.asset = PaletteAsset; });

    cubos.startupSystem("create colliders").tagged(assetsTag).call([](State& state, Commands commands, Assets& assets) {
        auto car = assets.read(CarAsset);
        glm::vec3 offset = glm::vec3(car->size().x, car->size().y, car->size().z) / -2.0F;
        state.a = commands.create()
                      .add(RenderVoxelGrid{CarAsset, offset})
                      .add(VoxelCollisionShape(CarAsset))
                      .add(CollisionLayers{})
                      .add(CollisionMask{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, -10.0F, -20.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 500.0F, .velocity = {0.0F, 0.0F, 1.0F}})
                      .entity();
        state.aRotationAxis = glm::sphericalRand(1.0F);

        state.b = commands.create()
                      .add(RenderVoxelGrid{CarAsset, offset})
                      .add(VoxelCollisionShape(CarAsset))
                      .add(CollisionLayers{})
                      .add(CollisionMask{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 15.0F, 10.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 500.0F,
                                         .velocity = {0.0F, 0.0F, -1.0F},
                                         .material = PhysicsMaterial{.bounciness = 0.0},
                                         .inertiaTensor = glm::mat3(0.0F)})
                      .entity();
        state.bRotationAxis = glm::sphericalRand(1.0F);
    });

    cubos.system("move colliders")
        .tagged(physicsApplyForcesTag)
        .call([](State& state, Query<Position&, Rotation&, Velocity&> query) {
            auto [aPos, aRot, aVel] = *query.at(state.a);
            auto [bPos, bRot, bVel] = *query.at(state.b);

            // aRot.quat = glm::rotate(aRot.quat, 0.001F, state.aRotationAxis);
            aVel.vec += glm::vec3{0.0F, 0.01F, 0.00F};

            // bRot.quat = glm::rotate(bRot.quat, 0.001F, state.bRotationAxis);
            bVel.vec -= glm::vec3{0.0F, 0.01F, 0.0F};
        });

    cubos.tag(collisionsSampleUpdated);

    cubos.system("render voxel boxes")
        .after(collisionsSampleUpdated)
        .call([](Gizmos& gizmos, Query<const LocalToWorld&, const VoxelCollisionShape&> query) {
            for (auto [localToWorld, shape] : query)
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
                    glm::mat4 transform = glm::scale(pos, size);
                    gizmos.drawWireBox("subboxes", transform);
                }
            }
        });

    cubos.system("render aabb")
        .after(collisionsSampleUpdated)
        .call([](Gizmos& gizmos, Query<const LocalToWorld&, const ColliderAABB&> query) {
            for (auto [localToWorld, colliderAABB] : query)
            {
                auto size = colliderAABB.localAABB.box().halfSize * 2.0F;
                glm::mat4 transform = glm::scale(localToWorld.mat, size);
                gizmos.color({1.0F, 1.0F, 1.0F});
                gizmos.drawWireBox("local AABB", transform);

                gizmos.color({1.0F, 0.0F, 0.0F});
                gizmos.drawWireBox("world AABB", colliderAABB.worldAABB.min(), colliderAABB.worldAABB.max());
            }
        });

    cubos.system("render collision manifolds")
        .after(collisionsTag)
        .after(gizmosDrawTag)
        .call([](Gizmos& gizmos, const DebugDraw& draw,
                 Query<Entity, const Position&, const CollidingWith&, Entity, const Position&> query) {
            for (auto [ent1, pos1, collidingWith, ent2, pos2] : query)
            {
                for (auto manifold : collidingWith.manifolds)
                {
                    if (draw.normal)
                    {
                        glm::vec3 origin = ent1 == collidingWith.entity ? pos1.vec : pos2.vec;
                        gizmos.color({0.0F, 1.0F, 0.0F});
                        gizmos.drawArrow("arrow", origin, manifold.normal, 0.1F, 0.5F, 0.7F, 0.05F,
                                         Gizmos::Space::World);
                    }

                    if (draw.manifoldPolygon && manifold.points.size() > 1)
                    {
                        cubos::engine::ContactPointData start = manifold.points.back();
                        for (const cubos::engine::ContactPointData& end : manifold.points)
                        {
                            gizmos.color({1.0F, 1.0F, 0.0F});
                            gizmos.drawArrow("line", start.globalOn1, end.globalOn1 - start.globalOn1, 1.0F, 1.2F, 1.0F,
                                             0.05F, Gizmos::Space::World);
                            start = end;
                        }
                    }

                    if (draw.points)
                    {
                        for (auto point : manifold.points)
                        {
                            gizmos.color({0.0F, 0.0F, 1.0F});
                            gizmos.drawArrow("point", point.globalOn1, glm::vec3(0.02F, 0.02F, 0.02F), 1.0F, 1.0F, 1.0F,
                                             0.05F, Gizmos::Space::World);

                            gizmos.color({1.0F, 0.0F, 1.0F});
                            gizmos.drawArrow("point", point.globalOn2, glm::vec3(0.02F, 0.02F, 0.02F), 1.0F, 1.0F, 1.0F,
                                             0.05F, Gizmos::Space::World);
                        }
                    }
                }
            }
        });

    cubos.run();
    return 0;
}
