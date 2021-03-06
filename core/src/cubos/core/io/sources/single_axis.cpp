#include <cubos/core/io/sources/single_axis.hpp>
#include <cubos/core/io/input_manager.hpp>

#include <tuple>
#include <variant>

using namespace cubos::core::io;

SingleAxis::SingleAxis(cubos::core::io::MouseAxis axis)
{
    this->inputs = axis;
}
SingleAxis::SingleAxis(cubos::core::io::Key negativeKey, cubos::core::io::Key positiveKey)
{
    this->inputs = std::make_tuple(negativeKey, positiveKey);
}

void SingleAxis::subscribeEvents()
{
    if (std::holds_alternative<cubos::core::io::MouseAxis>(this->inputs))
    {
        InputManager::registerMouseAxisCallback<SingleAxis>(this, &SingleAxis::handleAxis,
                                                            std::get<cubos::core::io::MouseAxis>(this->inputs));
    }
    else if (std::holds_alternative<std::tuple<cubos::core::io::Key, cubos::core::io::Key>>(this->inputs))
    {

        InputManager::registerKeyDownCallback<SingleAxis>(
            this, &SingleAxis::handleNegative,
            std::get<0>(std::get<std::tuple<cubos::core::io::Key, cubos::core::io::Key>>(this->inputs)));
        InputManager::registerKeyUpCallback<SingleAxis>(
            this, &SingleAxis::handlePositive,
            std::get<0>(std::get<std::tuple<cubos::core::io::Key, cubos::core::io::Key>>(this->inputs)));

        InputManager::registerKeyDownCallback<SingleAxis>(
            this, &SingleAxis::handlePositive,
            std::get<1>(std::get<std::tuple<cubos::core::io::Key, cubos::core::io::Key>>(this->inputs)));
        InputManager::registerKeyUpCallback<SingleAxis>(
            this, &SingleAxis::handleNegative,
            std::get<1>(std::get<std::tuple<cubos::core::io::Key, cubos::core::io::Key>>(this->inputs)));
    }
}

void SingleAxis::unsubscribeEvents()
{
    if (std::holds_alternative<cubos::core::io::MouseAxis>(this->inputs))
    {
        InputManager::unregisterMouseAxisCallback<SingleAxis>(this, &SingleAxis::handleAxis,
                                                              std::get<cubos::core::io::MouseAxis>(this->inputs));
    }
    else if (std::holds_alternative<std::tuple<cubos::core::io::Key, cubos::core::io::Key>>(this->inputs))
    {
        InputManager::unregisterKeyDownCallback<SingleAxis>(
            this, &SingleAxis::handleNegative,
            std::get<0>(std::get<std::tuple<cubos::core::io::Key, cubos::core::io::Key>>(this->inputs)));
        InputManager::unregisterKeyUpCallback<SingleAxis>(
            this, &SingleAxis::handlePositive,
            std::get<0>(std::get<std::tuple<cubos::core::io::Key, cubos::core::io::Key>>(this->inputs)));

        InputManager::unregisterKeyDownCallback<SingleAxis>(
            this, &SingleAxis::handlePositive,
            std::get<1>(std::get<std::tuple<cubos::core::io::Key, cubos::core::io::Key>>(this->inputs)));
        InputManager::unregisterKeyUpCallback<SingleAxis>(
            this, &SingleAxis::handleNegative,
            std::get<1>(std::get<std::tuple<cubos::core::io::Key, cubos::core::io::Key>>(this->inputs)));
    }
}

void SingleAxis::handleAxis(float value)
{
    this->value = value;
    this->wasTriggered = true;
}

void SingleAxis::handlePositive()
{
    this->value += 1;
    this->wasTriggered = true;
}

void SingleAxis::handleNegative()
{
    this->value -= 1;
    this->wasTriggered = true;
}

bool SingleAxis::isTriggered()
{
    if (this->wasTriggered)
    {
        this->wasTriggered = false;
        return true;
    }

    return false;
}

Context SingleAxis::createContext()
{
    return Context(float(this->value));
}
