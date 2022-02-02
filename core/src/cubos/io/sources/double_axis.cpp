#include <cubos/event.hpp>
#include <cubos/io/window.hpp>
#include <cubos/io/keyboard.hpp>
#include <map>
#include <string>
#include <memory>
#include <variant>
#include <list>
#include <glm/glm.hpp>
#include <cubos/io/input_manager.hpp>
#include <cubos/io/sources/double_axis.hpp>

using namespace cubos::io;

DoubleAxis::DoubleAxis(cubos::io::MouseAxis horizontalAxis, cubos::io::MouseAxis verticalAxis)
{
    this->horizontalAxis = horizontalAxis;
    this->verticalAxis = verticalAxis;
};

void DoubleAxis::subscribeEvents()
{
    if (std::holds_alternative<cubos::io::MouseAxis>(this->horizontalAxis))
    {
        InputManager::registerMouseAxisCallback<DoubleAxis>(this, &DoubleAxis::handleHorizontalAxis,
                                                            std::get<cubos::io::MouseAxis>(this->horizontalAxis));
    }

    if (std::holds_alternative<cubos::io::MouseAxis>(this->verticalAxis))
    {
        InputManager::registerMouseAxisCallback<DoubleAxis>(this, &DoubleAxis::handleVerticalAxis,
                                                            std::get<cubos::io::MouseAxis>(this->verticalAxis));
    }
};

void DoubleAxis::unsubscribeEvents()
{
    if (std::holds_alternative<cubos::io::MouseAxis>(this->horizontalAxis))
    {
        InputManager::unregisterMouseAxisCallback<DoubleAxis>(this, &DoubleAxis::handleHorizontalAxis,
                                                              std::get<cubos::io::MouseAxis>(this->horizontalAxis));
    }

    if (std::holds_alternative<cubos::io::MouseAxis>(this->verticalAxis))
    {
        InputManager::unregisterMouseAxisCallback<DoubleAxis>(this, &DoubleAxis::handleVerticalAxis,
                                                              std::get<cubos::io::MouseAxis>(this->verticalAxis));
    }
};

void DoubleAxis::handleHorizontalAxis(float xPos)
{
    this->xPos = xPos;
    this->wasTriggered = true;
}

void DoubleAxis::handleVerticalAxis(float yPos)
{
    this->yPos = yPos;
    this->wasTriggered = true;
}

bool DoubleAxis::isTriggered()
{
    if (DoubleAxis::wasTriggered)
    {
        DoubleAxis::wasTriggered = false;
        return true;
    }
    return false;
}

Context DoubleAxis::createContext()
{
    return Context(glm::vec2(this->xPos, this->yPos));
}
