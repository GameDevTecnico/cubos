#include <algorithm>

#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/transform/position.hpp>
#include <cubos/engine/transform/rotation.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::core::io;

void cubos::engine::freeCameraPlugin(Cubos& cubos)
{
    cubos.depends(inputPlugin);
    cubos.depends(windowPlugin);
    cubos.depends(transformPlugin);

    cubos.component<FreeCameraController>();

    cubos.system("rotate FreeCameraController on mouse motion")
        .call([](const Input& input, Window& window, const DeltaTime& deltaTime,
                 Query<FreeCameraController&, Position&, Rotation&> entities) {
            bool anyEnabled = false;

            for (auto [controller, position, rotation] : entities)
            {
                if (!controller.enabled)
                {
                    continue;
                }

                anyEnabled = true;

                auto dt = controller.unscaledDeltaTime ? deltaTime.unscaledValue : deltaTime.value();

                // Update camera angles based on mouse motion.
                controller.phi -= static_cast<float>(input.mouseDelta().y) * dt * controller.sens;
                controller.theta -= static_cast<float>(input.mouseDelta().x) * dt * controller.sens;
                controller.phi = std::clamp(controller.phi, -89.8F, 89.8F);

                // Calculate camera direction from angles and calculate the rotation from it.
                glm::vec3 forward{};
                forward.x = glm::cos(glm::radians(controller.phi)) * glm::sin(glm::radians(controller.theta));
                forward.y = glm::sin(glm::radians(controller.phi));
                forward.z = glm::cos(glm::radians(controller.phi)) * glm::cos(glm::radians(controller.theta));
                rotation.quat = glm::quatLookAt(forward, glm::vec3{0.0F, 1.0F, 0.0F});

                // Calculate right vector from forward and up.
                glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3{0.0F, 1.0F, 0.0F}));

                // Translate the camera based on input.
                auto factor = dt * controller.speed;
                position.vec += right * input.axis(controller.lateral.c_str()) * factor;
                position.vec.y += input.axis(controller.vertical.c_str()) * factor;
                position.vec += forward * input.axis(controller.longitudinal.c_str()) * factor;
            }

            // Lock/unlock mouse depending on if any FreeCameraController is enabled.
            if (anyEnabled && window->mouseState() != MouseState::Locked)
            {
                window->mouseState(MouseState::Locked);
            }
            else if (!anyEnabled && window->mouseState() != MouseState::Default)
            {
                window->mouseState(MouseState::Default);
            }
        });
}