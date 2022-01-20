#include <cubos/input/input_sources/single_axis.hpp>
#include <cubos/input/input_manager.hpp>
#include <tuple>
#include <variant>

using namespace cubos::input;

SingleAxis::SingleAxis(cubos::io::MouseAxis axis)
{
    this->inputs = axis;
}
SingleAxis::SingleAxis(cubos::io::Key negativeKey, cubos::io::Key positiveKey)
{
    this->inputs = std::make_tuple(negativeKey, positiveKey);
}

void SingleAxis::subscribeEvents(cubos::io::Window* window)
{
    if (std::holds_alternative<cubos::io::MouseAxis>(this->inputs))
    {
        InputManager::registerMouseAxisCallback<SingleAxis>(this, &SingleAxis::handleAxis,
                                                            std::get<cubos::io::MouseAxis>(this->inputs));
    }
    else if (std::holds_alternative<std::tuple<cubos::io::Key, cubos::io::Key>>(this->inputs))
    {

        InputManager::registerKeyDownCallback<SingleAxis>(
            this, &SingleAxis::handleNegative,
            std::get<0>(std::get<std::tuple<cubos::io::Key, cubos::io::Key>>(this->inputs)));

        InputManager::registerKeyDownCallback<SingleAxis>(
            this, &SingleAxis::handlePositive,
            std::get<1>(std::get<std::tuple<cubos::io::Key, cubos::io::Key>>(this->inputs)));
    }
}

void SingleAxis::unsubscribeEvents(cubos::io::Window* window)
{
    if (std::holds_alternative<cubos::io::MouseAxis>(this->inputs))
    {
        InputManager::unregisterMouseAxisCallback<SingleAxis>(this, &SingleAxis::handleAxis,
                                                              std::get<cubos::io::MouseAxis>(this->inputs));
    }
    else if (std::holds_alternative<std::tuple<cubos::io::Key, cubos::io::Key>>(this->inputs))
    {

        InputManager::unregisterKeyDownCallback<SingleAxis>(
            this, &SingleAxis::handleNegative,
            std::get<0>(std::get<std::tuple<cubos::io::Key, cubos::io::Key>>(this->inputs)));

        InputManager::unregisterKeyDownCallback<SingleAxis>(
            this, &SingleAxis::handlePositive,
            std::get<1>(std::get<std::tuple<cubos::io::Key, cubos::io::Key>>(this->inputs)));
    }
}

void SingleAxis::handleAxis(float value)
{
    this->value = value;
    this->wasTriggered = true;
}

void SingleAxis::handlePositive()
{
    if (this->wasTriggered)
    {
        this->value += 1;
    }
    else
    {
        this->value = 1;
        this->wasTriggered = true;
    }
}

void SingleAxis::handleNegative()
{
    if (this->wasTriggered)
    {
        this->value += -1;
    }
    else
    {
        this->value = -1;
        this->wasTriggered = true;
    }
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

InputContext SingleAxis::createInputContext()
{
    return InputContext(float(this->value));
}
