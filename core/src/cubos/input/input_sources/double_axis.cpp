#ifndef CUBOS_INPUT_DOUBLE_AXIS_CPP
#define CUBOS_INPUT_DOUBLE_AXIS_CPP

#include <cubos/event.hpp>
#include <cubos/io/window.hpp>
#include <cubos/io/keyboard.hpp>
#include <map>
#include <string>
#include <memory>
#include <variant>
#include <list>
#include <glm/glm.hpp>
#include <cubos/input/input_manager.hpp>

namespace cubos::input
{
    void DoubleAxis::subscribeEvents(cubos::io::Window* window)
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

    void DoubleAxis::unsubscribeEvents(cubos::io::Window* window)
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

    InputContext DoubleAxis::createInputContext()
    {
        return InputContext(glm::vec2(this->xPos, this->yPos));
    }
} // namespace cubos::input

#endif // CUBOS_INPUT_DOUBLE_AXIS_CPP