#include <glm/gtc/matrix_transform.hpp>

#include <cubos/engine/render/active/active.hpp>
#include <cubos/engine/render/active/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/orthographic.hpp>
#include <cubos/engine/render/camera/perspective.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>

using namespace cubos::engine;

void cubos::engine::cameraPlugin(Cubos& cubos)
{
    cubos.depends(renderTargetPlugin);
    cubos.depends(activePlugin);

    cubos.component<Camera>();
    cubos.component<PerspectiveCamera>();
    cubos.component<OrthographicCamera>();

    cubos.relation<DrawsTo>();

    cubos.observer("add active component on add Camera")
        .onAdd<Camera>()
        .without<Active>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [ent] : query)
            {
                cmds.add(ent, Active{});
            }
        });

    cubos.observer("add Camera on add PerspectiveCamera")
        .onAdd<PerspectiveCamera>()
        .without<Camera>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [ent] : query)
            {
                cmds.add(ent, Camera{});
            }
        });

    cubos.observer("add Camera on add OrthographicCamera")
        .onAdd<OrthographicCamera>()
        .without<Camera>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [ent] : query)
            {
                cmds.add(ent, Camera{});
            }
        });

    cubos.system("update Camera projection by PerspectiveCamera")
        .call([](Query<Camera&, const PerspectiveCamera&, const DrawsTo&, const RenderTarget&> query) {
            for (auto [camera, perspective, drawsTo, target] : query)
            {
                float aspect = (static_cast<float>(target.size.x) * drawsTo.viewportSize.x) /
                               (static_cast<float>(target.size.y) * drawsTo.viewportSize.y);
                if (aspect > 0)
                {
                    camera.projection =
                        glm::perspective(glm::radians(perspective.fovY), aspect, camera.zNear, camera.zFar);
                }
            }
        });

    cubos.system("update Camera projection by OrthographicCamera")
        .call([](Query<Camera&, const OrthographicCamera&, const DrawsTo&, const RenderTarget&> query) {
            for (auto [camera, ortho, drawsTo, target] : query)
            {
                float aspect = (static_cast<float>(target.size.x) * drawsTo.viewportSize.x) /
                               (static_cast<float>(target.size.y) * drawsTo.viewportSize.y);
                if (ortho.axis == OrthographicCamera::Axis::Vertical)
                {
                    camera.projection = glm::ortho(-ortho.size * aspect, ortho.size * aspect, -ortho.size, ortho.size,
                                                   camera.zNear, camera.zFar);
                }
                else
                {
                    camera.projection = glm::ortho(-ortho.size, ortho.size, -ortho.size / aspect, ortho.size / aspect,
                                                   camera.zNear, camera.zFar);
                }
            }
        });
}
