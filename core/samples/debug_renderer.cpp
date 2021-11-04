#include <cubos/log.hpp>
#include <cubos/io/window.hpp>
#include <cubos/gl/render_device.hpp>
#include <cubos/debug_render.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace cubos;

void debugTest(io::Key key)
{
    if (key == io::Key::C)
        Debug::drawWireCube(glm::vec3(0, 0, 0), glm::vec3(1), 1);
    else if (key == io::Key::S)
        Debug::drawWireSphere(glm::vec3(0, 0, 0), 1, 1);
}

int main(void)
{
    initializeLogger();
    auto* window = io::Window::create();
    auto& renderDevice = window->getRenderDevice();
    Debug::init(window);

    window->onKeyDown.registerCallback(debugTest);

    float prevT = -1;

    while (!window->shouldClose())
    {
        float t = window->getTime();
        if (prevT < 0)
            prevT = t;
        double deltaT = t - prevT;
        prevT = t;
        renderDevice.clearColor(0.0, 0.0, 0.0, 0.0f);

        auto sz = window->getFramebufferSize();

        auto vp = glm::perspective(glm::radians(70.0f), float(sz.x) / float(sz.y), 0.1f, 1000.0f) *
                  glm::lookAt(glm::vec3{4.0f, 3.0f, -5.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});

        Debug::flush(vp, deltaT);
        window->swapBuffers();
        window->pollEvents();
    }

    delete window;
    return 0;
}
