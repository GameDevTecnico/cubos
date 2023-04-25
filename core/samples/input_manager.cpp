#include <algorithm>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/io/input_manager.hpp>
#include <cubos/core/io/sources/button_press.hpp>
#include <cubos/core/io/sources/double_axis.hpp>
#include <cubos/core/io/sources/single_axis.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/log.hpp>
using namespace cubos::core;
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
    auto window = io::openWindow();
    auto& renderDevice = window->getRenderDevice();

    height = window->getFramebufferSize().y;
    width = window->getFramebufferSize().x;

    io::InputManager::init(window);
    auto updateRedGreenAction = io::InputManager::createAction("update_red_green");
    updateRedGreenAction->addBinding(updateColour);
    updateRedGreenAction->addSource(new io::DoubleAxis(cubos::core::io::MouseAxis::X, cubos::core::io::MouseAxis::Y));

    auto incBlueAction = io::InputManager::createAction("increase_blue");
    incBlueAction->addBinding(blueIncr);
    incBlueAction->addSource(new io::ButtonPress(io::Key::D));
    incBlueAction->addSource(new io::ButtonPress(io::MouseButton::Right));

    auto decBlueAction = io::InputManager::createAction("decrease_blue");
    decBlueAction->addBinding(blueDecr);
    decBlueAction->addSource(new io::ButtonPress(io::Key::A));
    decBlueAction->addSource(new io::ButtonPress(io::MouseButton::Left));

    auto updateAlphaAction = io::InputManager::createAction("update_alpha");
    updateAlphaAction->addBinding(updateToBlack);
    updateAlphaAction->addSource(new io::SingleAxis(io::Key::O, io::Key::L));

    while (!window->shouldClose())
    {
        renderDevice.clearColor(red, green, blue, 0.0f);
        window->swapBuffers();
        window->pollEvents();
        io::InputManager::processActions();
    }

    return 0;
}
