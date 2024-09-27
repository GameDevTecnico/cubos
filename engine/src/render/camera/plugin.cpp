#include <glm/gtc/matrix_transform.hpp>

#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/perspective_camera.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>

using namespace cubos::engine;

void cubos::engine::cameraPlugin(Cubos& cubos)
{
    cubos.depends(renderTargetPlugin);

    cubos.component<PerspectiveCamera>();
    cubos.component<Camera>();

    cubos.relation<DrawsTo>();

    cubos.observer("add Camera on add PerspectiveCamera")
        .onAdd<PerspectiveCamera>()
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
                        glm::perspective(glm::radians(perspective.fovY), aspect, perspective.zNear, perspective.zFar);
                }
            }
        });
}
