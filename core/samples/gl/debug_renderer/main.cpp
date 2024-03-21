#include <glm/gtc/matrix_transform.hpp>

#include <cubos/core/gl/debug.hpp>
#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/log.hpp>

using namespace cubos::core;

int main()
{
    auto window = io::openWindow();
    auto& renderDevice = window->renderDevice();
    gl::Debug::init(renderDevice);

    double prevT = -1;

    while (!window->shouldClose())
    {
        double t = window->time();
        if (prevT < 0)
        {
            prevT = t;
        }
        double deltaT = t - prevT;
        prevT = t;

        auto sz = window->framebufferSize();
        renderDevice.clearColor(0.0, 0.0, 0.0, 0.0F);
        renderDevice.setViewport(0, 0, static_cast<int>(sz.x), static_cast<int>(sz.y));

        auto vp = glm::perspective(glm::radians(70.0F), float(sz.x) / float(sz.y), 0.1F, 1000.0F) *
                  glm::lookAt(glm::vec3{3 * sinf((float)t), 3, 3 * cosf((float)t)}, glm::vec3{0.0F, 0.0F, 0.0F},
                              glm::vec3{0.0F, 1.0F, 0.0F});

        // Gizmo
        gl::Debug::drawLine(glm::vec3{0.0F}, glm::vec3{1.0F, 0.0F, 0.0F}, true, glm::vec3{1.0F, 0.0F, 0.0F});
        gl::Debug::drawLine(glm::vec3{0.0F}, glm::vec3{0.0F, 1.0F, 0.0F}, true, glm::vec3{0.0F, 1.0F, 0.0F});
        gl::Debug::drawLine(glm::vec3{0.0F}, glm::vec3{0.0F, 0.0F, 1.0F}, true, glm::vec3{0.0F, 0.0F, 1.0F});

        gl::Debug::flush(vp, deltaT);
        window->swapBuffers();
        while (auto event = window->pollEvent())
        {
            if (std::holds_alternative<io::KeyEvent>(event.value()))
            {
                auto keyEvent = std::get<io::KeyEvent>(event.value());
                if (keyEvent.key == io::Key::C && keyEvent.pressed)
                {
                    gl::Debug::drawWireBox(geom::Box{}, glm::mat4{1.0}, glm::vec3{1.0}, 1);
                }
                else if (keyEvent.key == io::Key::S && keyEvent.pressed)
                {
                    gl::Debug::drawWireSphere(glm::vec3(0, 0, 0), 1, 1);
                }
            }
        }
    }

    gl::Debug::terminate();
    return 0;
}
