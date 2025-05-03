#include <glm/gtc/random.hpp>

#include <cubos/core/geom/box.hpp>
#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/collider_bundle.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/collision_layers.hpp>
#include <cubos/engine/collisions/collision_mask.hpp>
#include <cubos/engine/collisions/contact_manifold.hpp>
#include <cubos/engine/collisions/intersection_end.hpp>
#include <cubos/engine/collisions/intersection_start.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/gizmos/target.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/perspective.hpp>
#include <cubos/engine/render/defaults/plugin.hpp>
#include <cubos/engine/render/defaults/target.hpp>
#include <cubos/engine/render/tone_mapping/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

#include "../src/collisions/broad_phase/collider_aabb.hpp"

using cubos::core::geom::Box;
using cubos::core::io::Key;
using cubos::core::io::Modifiers;

using namespace cubos::engine;

static CUBOS_DEFINE_TAG(collisionsSampleUpdated);

struct Options
{
    CUBOS_ANONYMOUS_REFLECT(Options);

    bool useVoxelShape = true;
};

struct State
{
    CUBOS_ANONYMOUS_REFLECT(State);
    Entity a;
    Entity b;

    glm::vec3 aColor = {1.0F, 1.0F, 1.0F};
    glm::vec3 bColor = {1.0F, 1.0F, 1.0F};
};

static const Asset<InputBindings> BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a331d7842e5");
static const Asset<VoxelGrid> CarAsset = AnyAsset("059c16e7-a439-44c7-9bdc-6e069dba0c75");

int main(int argc, char** argv)
{
    auto cubos = Cubos(argc, argv);

    cubos.plugin(defaultsPlugin);
    cubos.plugin(freeCameraPlugin);
    cubos.tag(gizmosDrawTag).after(toneMappingTag);

    cubos.resource<State>();
    cubos.resource<Options>();

    cubos.startupSystem("configure Assets").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
    });

    cubos.startupSystem("setup input").tagged(assetsTag).call([](const Assets& assets, Input& input) {
        auto bindings = assets.read<InputBindings>(BindingsAsset);

        input.bind(*bindings);
    });

    cubos.startupSystem("setup camera").call([](Commands commands) {
        auto targetEnt = commands.create().add(RenderTargetDefaults{}).add(GizmosTarget{}).entity();

        commands.create()
            .relatedTo(targetEnt, DrawsTo{})
            .add(Camera{.zNear = 0.1F, .zFar = 100.0F})
            .add(PerspectiveCamera{.fovY = 60.0F})
            .add(LocalToWorld{})
            .add(Position{{-4.0F, 1.5F, 0.0F}})
            .add(Rotation{Rotation::lookingAt({3.0F, -1.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F})})
            .add(FreeCameraController{.phi = -15.0F, .theta = 90.0F});
    });

    cubos.startupSystem("create colliders")
        .tagged(assetsTag)
        .call([](Options& options, State& state, Commands commands, Assets& assets) {
            if (!options.useVoxelShape)
            {
                state.a = commands.create()
                              .add(BoxCollisionShape{})
                              .add(LocalToWorld{})
                              .add(Position{glm::vec3{0.0F, 0.0F, -1.0F}})
                              .add(Rotation{})
                              .add(ColliderBundle{.isArea = true})
                              .entity();
            }
            else
            {
                auto car = assets.read<VoxelGrid>(CarAsset);
                glm::vec3 offset = glm::vec3(car->size().x, car->size().y, car->size().z) / -2.0F;
                state.a = commands.create()
                              .add(VoxelCollisionShape(CarAsset))
                              .add(LocalToWorld{})
                              .add(Position{glm::vec3{0.0F, 0.0F, -20.0F}})
                              .add(Rotation{})
                              .add(ColliderBundle{.isArea = true})
                              .entity();
            }

            state.b = commands.create()
                          .add(BoxCollisionShape{})
                          .add(LocalToWorld{})
                          .add(Position{glm::vec3{0.0F, 0.0F, 1.0F}})
                          .add(Rotation{})
                          .add(ColliderBundle{.isArea = true})
                          .entity();
        });

    cubos.system("activate free camera").call([](const Input& input, Query<FreeCameraController&> query) {
        for (auto [controller] : query)
        {
            if (input.justPressed("camera-toggle"))
            {
                controller.enabled = !controller.enabled;
            }
        }
    });

    cubos.system("move colliders")
        .tagged(physicsApplyForcesTag)
        .call([](State& state, const Input& input, Query<Position&, Collider&> query) {
            auto [aPos, aCol] = *query.at(state.a);
            auto [bPos, bCol] = *query.at(state.b);

            if (input.justPressed("reset"))
            {
                aCol.isActive = !aCol.isActive;
                CUBOS_INFO("Switch Activate Collider A");
            }

            aPos.vec +=
                glm::vec3(input.axis("x-axis") * 0.1F, input.axis("y-axis") * 0.1F, input.axis("z-axis") * 0.1F);
        });

    cubos.tag(collisionsSampleUpdated).after(collisionsTag);

    cubos.system("check collisions")
        .tagged(collisionsSampleUpdated)
        .call([](Query<Entity, Collider&, IntersectionStart&, Entity, Collider&> query, State& state) {
            for (auto [ent1, collider1, intersectionStart, ent2, collider2] : query)
            {
                if ((ent1 == state.a && ent2 == state.b) || (ent1 == state.b && ent2 == state.a))
                {
                    CUBOS_INFO("Intersected");
                    if (collider1.isArea)
                    {
                        if (ent1 == state.a)
                        {
                            state.aColor = {0.0F, 0.0F, 1.0F};
                        }
                        else
                        {
                            state.bColor = {0.0F, 0.0F, 1.0F};
                        }
                    }
                    if (collider2.isArea)
                    {
                        if (ent2 == state.a)
                        {
                            state.aColor = {0.0F, 0.0F, 1.0F};
                        }
                        else
                        {
                            state.bColor = {0.0F, 0.0F, 1.0F};
                        }
                    }
                }
            }
        });

    cubos.system("check collisions")
        .tagged(collisionsSampleUpdated)
        .call([](Query<Entity, Collider&, IntersectionEnd&, Entity, Collider&> query, State& state) {
            for (auto [ent1, collider1, intersectionEnd, ent2, collider2] : query)
            {
                if ((ent1 == state.a && ent2 == state.b) || (ent1 == state.b && ent2 == state.a))
                {
                    CUBOS_INFO("Stoped Intersecting");
                    if (collider1.isArea)
                    {
                        if (ent1 == state.a)
                        {
                            state.aColor = {1.0F, 0.0F, 0.0F};
                        }
                        else
                        {
                            state.bColor = {1.0F, 0.0F, 0.0F};
                        }
                    }
                    if (collider2.isArea)
                    {
                        if (ent2 == state.a)
                        {
                            state.aColor = {1.0F, 0.0F, 0.0F};
                        }
                        else
                        {
                            state.bColor = {1.0F, 0.0F, 0.0F};
                        }
                    }
                }
            }
        });

    cubos.system("check collisions")
        .tagged(collisionsSampleUpdated)
        .call([](Query<Entity, Collider&, CollidingWith&, Entity, Collider&> query, State& state) {
            for (auto [ent1, collider1, colliding, ent2, collider2] : query)
            {
                if ((ent1 == state.a && ent2 == state.b) || (ent1 == state.b && ent2 == state.a))
                {
                    if (collider1.isArea)
                    {
                        if (ent1 == state.a)
                        {
                            state.aColor = {0.0F, 1.0F, 0.0F};
                        }
                        else
                        {
                            state.bColor = {0.0F, 1.0F, 0.0F};
                        }
                    }
                    if (collider2.isArea)
                    {
                        if (ent2 == state.a)
                        {
                            state.aColor = {0.0F, 1.0F, 0.0F};
                        }
                        else
                        {
                            state.bColor = {0.0F, 1.0F, 0.0F};
                        }
                    }
                }
            }
        });

    cubos.system("render")
        .after(collisionsSampleUpdated)
        .after(gizmosDrawTag)
        .call([](Gizmos& gizmos, State& state, Query<Entity, const LocalToWorld&, const ColliderAABB&> query) {
            for (auto [ent, localToWorld, colliderAABB] : query)
            {
                glm::vec3 color = ent == state.a ? state.aColor : state.bColor;

                auto size = colliderAABB.localAABB.box().halfSize * 2.0F;
                glm::mat4 transform = glm::scale(localToWorld.mat, size);
                gizmos.color(color);
                gizmos.drawWireBox("local AABB", transform);
            }
        });

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

    cubos.run();
    return 0;
}
