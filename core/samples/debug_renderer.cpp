#include <glm/gtc/matrix_transform.hpp>

#include <cubos/core/log.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/gl/debug.hpp>

using namespace cubos::core;

void debugTest(io::Key key)
{
    if (key == io::Key::C)
        gl::Debug::drawWireCube(glm::vec3(0, 0, 0), glm::vec3(1), 1);
    else if (key == io::Key::S)
        gl::Debug::drawWireSphere(glm::vec3(0, 0, 0), 1, 1);
}

int main(void)
{
    initializeLogger();
    auto* window = io::Window::create();
    auto& renderDevice = window->getRenderDevice();
    gl::Debug::init(renderDevice);

    window->onKeyDown.registerCallback(debugTest);

    double prevT = -1;

    while (!window->shouldClose())
    {
        double t = window->getTime();
        if (prevT < 0)
            prevT = t;
        double deltaT = t - prevT;
        prevT = t;

        auto sz = window->getFramebufferSize();
        renderDevice.clearColor(0.0, 0.0, 0.0, 0.0f);
        renderDevice.setViewport(0, 0, sz.x, sz.y);

        auto vp = glm::perspective(glm::radians(70.0f), float(sz.x) / float(sz.y), 0.1f, 1000.0f) *
                  glm::lookAt(glm::vec3{3 * sinf((float)t), 3, 3 * cosf((float)t)}, glm::vec3{0.0f, 0.0f, 0.0f},
                              glm::vec3{0.0f, 1.0f, 0.0f});

        gl::Debug::drawWireCube({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 0.0);
        gl::Debug::flush(vp, deltaT);
        window->swapBuffers();
        window->pollEvents();
    }

    gl::Debug::terminate();
    delete window;
    return 0;
}
