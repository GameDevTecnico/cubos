#include <glm/gtc/matrix_transform.hpp>

#include <cubos/core/memory/opt.hpp>
#include <cubos/core/reflection/external/glm.hpp>

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
        .call([](Query<Camera&, const PerspectiveCamera&, const LocalToWorld&, const DrawsTo&, const RenderTarget&>
                     query) {
            for (auto [camera, perspective, localToWorld, drawsTo, target] : query)
            {
                if (camera.active && !camera.freezeFrustum)
                {
                    glm::vec3 up = localToWorld.up();
                    glm::vec3 front = -localToWorld.forward();
                    glm::vec3 right = glm::normalize(glm::cross(front, up));
                    glm::vec3 position = localToWorld.worldPosition();

                    float aspect = (static_cast<float>(target.size.x) * drawsTo.viewportSize.x) /
                                   (static_cast<float>(target.size.y) * drawsTo.viewportSize.y);
                    float halfVSide = camera.zFar * tanf(glm::radians(perspective.fovY) * 0.5F);
                    float halfHSide = halfVSide * aspect;

                    glm::vec3 nearCenter = position + camera.zNear * front;
                    glm::vec3 farCenter = position + camera.zFar * front;
                    glm::vec3 bottomLeft = farCenter - right * halfHSide - up * halfVSide;
                    glm::vec3 bottomRight = farCenter + right * halfHSide - up * halfVSide;
                    glm::vec3 topLeft = farCenter - right * halfHSide + up * halfVSide;
                    glm::vec3 topRight = farCenter + right * halfHSide + up * halfVSide;

                    glm::vec3 rightNorm = glm::normalize(glm::cross(position - topRight, position - bottomRight));
                    glm::vec3 leftNorm = glm::normalize(glm::cross(position - bottomLeft, position - topLeft));
                    glm::vec3 topNorm = glm::normalize(glm::cross(position - topLeft, position - topRight));
                    glm::vec3 botNorm = glm::normalize(glm::cross(position - bottomRight, position - bottomLeft));

                    camera.frustum.near = {.normal = front, .d = -glm::dot(front, nearCenter)};
                    camera.frustum.far = {.normal = -front, .d = -glm::dot(-front, farCenter)};
                    camera.frustum.top = {.normal = topNorm, .d = -glm::dot(topNorm, position)};
                    camera.frustum.bottom = {.normal = botNorm, .d = -glm::dot(botNorm, position)};
                    camera.frustum.left = {.normal = leftNorm, .d = -glm::dot(leftNorm, position)};
                    camera.frustum.right = {.normal = rightNorm, .d = -glm::dot(rightNorm, position)};
                }
            }
        });

    cubos.system("update Camera frustum by OrthographicCamera")
        .call([](Query<Camera&, const OrthographicCamera&, const LocalToWorld&, const DrawsTo&, const RenderTarget&>
                     query) {
            for (auto [camera, ortho, localToWorld, drawsTo, target] : query)
            {
                if (camera.active && !camera.freezeFrustum)
                {
                    glm::vec3 up = localToWorld.up();
                    glm::vec3 front = -localToWorld.forward();
                    glm::vec3 right = glm::normalize(glm::cross(front, up));
                    glm::vec3 position = localToWorld.worldPosition();

                    float aspect = (static_cast<float>(target.size.x) * drawsTo.viewportSize.x) /
                                   (static_cast<float>(target.size.y) * drawsTo.viewportSize.y);
                    float horizontal = ortho.size;
                    float vertical = ortho.size;
                    if (ortho.axis == OrthographicCamera::Axis::Vertical)
                    {
                        horizontal *= aspect;
                    }
                    else
                    {
                        vertical /= aspect;
                    }

                    glm::vec3 nearPoint = position + camera.zNear * front;
                    glm::vec3 farPoint = position + camera.zFar * front;
                    glm::vec3 rightPoint = position + horizontal * right;
                    glm::vec3 leftPoint = position + horizontal * -right;
                    glm::vec3 topPoint = position + vertical * up;
                    glm::vec3 bottomPoint = position + vertical * -up;

                    camera.frustum.near = {.normal = front, .d = -glm::dot(front, nearPoint)};
                    camera.frustum.far = {.normal = -front, .d = -glm::dot(-front, farPoint)};
                    camera.frustum.right = {.normal = -right, .d = -glm::dot(-right, rightPoint)};
                    camera.frustum.left = {.normal = right, .d = -glm::dot(right, leftPoint)};
                    camera.frustum.top = {.normal = -up, .d = -glm::dot(-up, topPoint)};
                    camera.frustum.bottom = {.normal = up, .d = -glm::dot(up, bottomPoint)};
                }
            }
        });
}
