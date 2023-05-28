#include <cubos/core/log.hpp>

#include <cubos/engine/input/input.hpp>

using namespace cubos::engine;

void Input::clear()
{
    // TODO: Deregister from all events
    mBindings.clear();
    CUBOS_DEBUG("Input cleared");
}

void Input::clear(int player)
{
    // TODO: Deregister from all the events for this player
    mBindings.erase(player);
    CUBOS_DEBUG("Input cleared for player {}", player);
}

void Input::setBindings(int player, const Bindings& bindings)
{
    // TODO: Register to all the events for this player
    mBindings[player] = std::move(bindings);
    CUBOS_DEBUG("Input bindings set for player {}", player);
}

bool Input::isPressed(int player, const std::string& actionName) const
{
    auto pIt = mBindings.find(player);
    if (pIt == mBindings.end())
    {
        CUBOS_WARN("Player {} does not exist", player);
        return false;
    }

    auto aIt = pIt->second.getActions().find(actionName);
    if (aIt == pIt->second.getActions().end())
    {
        CUBOS_WARN("Action {} does not exist", actionName);
        return false;
    }

    return aIt->second.isPressed();
}

float Input::axis(int player, const std::string& axisName) const
{
    auto pIt = mBindings.find(player);
    if (pIt == mBindings.end())
    {
        CUBOS_WARN("Player {} does not exist", player);
        return 0.0f;
    }

    auto aIt = pIt->second.getAxes().find(axisName);
    if (aIt == pIt->second.getAxes().end())
    {
        CUBOS_WARN("Axis {} does not exist", axisName);
        return 0.0f;
    }

    return aIt->second.getValue();
}

const std::unordered_map<int, Bindings>& Input::getBindings() const
{
    return mBindings;
}