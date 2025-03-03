#include <glm/gtc/matrix_transform.hpp>

#include <cubos/core/memory/opt.hpp>

#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/orthographic.hpp>
#include <cubos/engine/render/camera/perspective.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

void cubos::engine::cameraPlugin(Cubos& cubos)
{
    cubos.depends(renderTargetPlugin);
    cubos.depends(transformPlugin);

    cubos.component<Camera>();
    cubos.component<PerspectiveCamera>();
    cubos.component<OrthographicCamera>();

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

    cubos.system("update Camera frustum by PerspectiveCamera")
        .call([](Query<Camera&, const PerspectiveCamera&, Opt<const LocalToWorld&>> query) {
            for (auto [camera, perspective, optLocalToWorld] : query)
            {
                if (camera.active)
                {
                    (void)perspective;
                    glm::mat4 view{1.0f};
                    if (optLocalToWorld.contains())
                    {
                        view = optLocalToWorld.value().mat;
                    }
                    glm::vec up = glm::normalize(glm::vec3{view[0][1], view[1][1], view[2][1]});
                    glm::vec3 right = glm::normalize(glm::vec3{view[0][0], view[1][0], view[2][0]});
                    glm::vec3 front = glm::normalize(glm::vec3{view[0][2], view[1][2], view[2][2]});
                    glm::vec3 position = {view[0][3], view[1][3], view[2][3]};

                    const float halfVSide = camera.zFar * tanf(perspective.fovY * .5f);
                    float aspect = camera.projection[1][1] / camera.projection[0][0];
                    const float halfHSide = halfVSide * aspect;
                    CUBOS_WARN("Position: {} {} {}", position.x, position.y, position.z);
                    CUBOS_WARN("UP: {} {} {}", up.x, up.y, up.z);
                    CUBOS_WARN("Right: {} {} {}", right.x, right.y, right.z);
                    CUBOS_WARN("FRONT: {} {} {}", front.x, front.y, front.z);

                    glm::vec3 nearPoint = position + camera.zNear * front;
                    glm::vec3 farPoint = position + camera.zFar * front;
                    glm::vec3 rightNorm = glm::normalize(glm::cross(camera.zFar * front - right * halfHSide, up));
                    glm::vec3 leftNorm = glm::normalize(glm::cross(up, camera.zFar * front + right * halfHSide));
                    glm::vec3 topNorm = glm::normalize(glm::cross(right, camera.zFar * front - up * halfVSide));
                    glm::vec3 botNorm = glm::normalize(glm::cross(camera.zFar * front + up * halfVSide, right));
                    (void)farPoint;
                    (void)rightNorm;
                    (void)leftNorm;
                    (void)topNorm;
                    (void)botNorm;

                    camera.frustum.near = {front, glm::dot(front, nearPoint)};
                    camera.frustum.far = camera.frustum.near;
                    camera.frustum.right = camera.frustum.near;
                    camera.frustum.left = camera.frustum.near;
                    camera.frustum.top = camera.frustum.near;
                    camera.frustum.bottom = camera.frustum.near;

                    // camera.frustum.far = {-front, glm::dot(-front, farPoint)};
                    // camera.frustum.right = {rightNorm, glm::dot(rightNorm, position)};
                    // camera.frustum.left = {leftNorm, glm::dot(leftNorm, position)};
                    // camera.frustum.top = {topNorm, glm::dot(topNorm, position)};
                    // camera.frustum.bottom = {botNorm, glm::dot(botNorm, position)};
                }
            }
        });

    cubos.system("update Camera frustum by OrthographicCamera")
        .call([](Query<Camera&, const OrthographicCamera&> query) {
            for (auto [camera, perspective] : query)
            {
                (void)camera;
                (void)perspective;
                (void)query;
            }
        });
}
