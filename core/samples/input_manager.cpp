#include <cubos/log.hpp>
#include <cubos/io/window.hpp>
#include <cubos/gl/render_device.hpp>
#include <cubos/io/input_manager.hpp>
#include <cubos/io/sources/button_press.hpp>
#include <cubos/io/sources/single_axis.hpp>
#include <cubos/io/sources/double_axis.hpp>
#include <algorithm>
using namespace cubos;
float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
float alpha = 1.0f;
int width = 0;
int height = 0;

void updateColour(io::Context context)
{
    red = std::clamp(context.getValue<glm::vec2>().x / width, 0.0f, 1.0f);
    green = std::clamp(context.getValue<glm::vec2>().y / width, 0.0f, 1.0f);
}

void blueIncr(io::Context context)
{
    blue += 0.1f;
    if (blue < 0)
    {
        blue = 0;
    }
}

void blueDecr(io::Context context)
{
    blue -= 0.1f;
    if (blue < 0)
    {
        blue = 0;
    }
}

void updateToBlack(io::Context context)
{
    red = std::clamp(red + context.getValue<float>() * 0.05f, 0.0f, 1.0f);
    green = std::clamp(green + context.getValue<float>() * 0.05f, 0.0f, 1.0f);
    blue = std::clamp(blue + context.getValue<float>() * 0.05f, 0.0f, 1.0f);
}

int main(void)
{
    initializeLogger();
    auto window = io::Window::create();
    auto& renderDevice = window->getRenderDevice();

    height = window->getFramebufferSize().y;
    width = window->getFramebufferSize().x;

    io::InputManager::init(window);
    auto updateRedGreenAction = io::InputManager::createAction("update_red_green");
    updateRedGreenAction->addBinding(updateColour);
    updateRedGreenAction->addInput(new io::DoubleAxis(cubos::io::MouseAxis::X, cubos::io::MouseAxis::Y));

    auto incBlueAction = io::InputManager::createAction("increase_blue");
    incBlueAction->addBinding(blueIncr);
    incBlueAction->addInput(new io::ButtonPress(io::Key::D));
    incBlueAction->addInput(new io::ButtonPress(io::MouseButton::Right));

    auto decBlueAction = io::InputManager::createAction("decrease_blue");
    decBlueAction->addBinding(blueDecr);
    decBlueAction->addInput(new io::ButtonPress(io::Key::A));
    decBlueAction->addInput(new io::ButtonPress(io::MouseButton::Left));

    auto updateAlphaAction = io::InputManager::createAction("update_alpha");
    updateAlphaAction->addBinding(updateToBlack);
    updateAlphaAction->addInput(new io::SingleAxis(io::Key::O, io::Key::L));

    while (!window->shouldClose())
    {
        renderDevice.clearColor(red, green, blue, 0.0f);
        window->swapBuffers();
        window->pollEvents();
        io::InputManager::processActions();
    }

    delete window;
    return 0;
}
