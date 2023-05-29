/// @file
#include <cubos/core/data/debug_serializer.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/input/input.hpp>

using cubos::core::data::Debug;
using cubos::core::io::KeyEvent;
using cubos::core::io::ModifiersEvent;
using namespace cubos::engine;

void Input::clear()
{
    mBoundActions.clear();
    mBoundAxes.clear();
    mBindings.clear();
    CUBOS_DEBUG("Input bindings cleared");
}

void Input::removeBoundPlayer(std::vector<BindingIndex>& boundKeys, int player)
{
    boundKeys.erase(
        std::remove_if(boundKeys.begin(), boundKeys.end(), [player](const auto& idx) { return idx.player == player; }),
        boundKeys.end());
}

void Input::clear(int player)
{
    for (const auto& action : mBindings[player].actions())
    {
        for (const auto& key : action.second.keys())
        {
            removeBoundPlayer(mBoundActions[key.key()], player);
        }
    }

    for (const auto& axis : mBindings[player].axes())
    {
        for (const auto& pos : axis.second.positive())
        {
            removeBoundPlayer(mBoundAxes[pos.key()], player);
        }

        for (const auto& neg : axis.second.negative())
        {
            removeBoundPlayer(mBoundAxes[neg.key()], player);
        }
    }

    mBindings.erase(player);
    CUBOS_DEBUG("Input bindings cleared for player {}", player);
}

void Input::bind(const InputBindings& bindings, int player)
{
    clear(player);

    for (const auto& action : bindings.actions())
    {
        for (const auto& key : action.second.keys())
        {
            mBoundActions[key.key()].push_back(BindingIndex{action.first, player});
        }
    }

    for (const auto& axis : bindings.axes())
    {
        for (const auto& pos : axis.second.positive())
        {
            mBoundAxes[pos.key()].push_back(BindingIndex{axis.first, player, false});
        }

        for (const auto& neg : axis.second.negative())
        {
            mBoundAxes[neg.key()].push_back(BindingIndex{axis.first, player, true});
        }
    }

    mBindings[player] = bindings;
    CUBOS_DEBUG("Input bindings set for player {}", player);
}

bool Input::pressed(const std::string& actionName, int player) const
{
    auto pIt = mBindings.find(player);
    if (pIt == mBindings.end())
    {
        CUBOS_WARN("Player {} does not have any associated input bindings", player);
        return false;
    }

    auto aIt = pIt->second.actions().find(actionName);
    if (aIt == pIt->second.actions().end())
    {
        CUBOS_WARN("Action {} is not bound to any input for player {}", actionName, player);
        return false;
    }

    return aIt->second.pressed();
}

float Input::axis(const std::string& axisName, int player) const
{
    auto pIt = mBindings.find(player);
    if (pIt == mBindings.end())
    {
        CUBOS_WARN("Player {} does not have any associated input bindings", player);
        return 0.0f;
    }

    auto aIt = pIt->second.axes().find(axisName);
    if (aIt == pIt->second.axes().end())
    {
        CUBOS_WARN("Axis {} is not bound to any input for player {}", axisName, player);
        return 0.0f;
    }

    return aIt->second.value();
}

void Input::handleKeys(std::vector<KeyWithModifiers>& keys, const KeyEvent& event)
{
    for (auto& key : keys)
    {
        if (event.key == key.key() && mModifiers == key.modifiers())
        {
            key.pressed(event.pressed);
        }
        else if (!event.pressed && event.key == key.key())
        {
            /// Release even if the modifier is different.
            key.pressed(false);
        }
    }
}

bool Input::anyPressed(std::vector<KeyWithModifiers>& keys) const
{
    for (auto& key : keys)
    {
        if (key.pressed())
            return true;
    }

    return false;
}

void Input::handle(const KeyEvent& event)
{
    for (const auto& boundAction : mBoundActions[event.key])
    {
        auto& action = mBindings[boundAction.player].actions()[boundAction.name];

        handleKeys(action.keys(), event);

        bool pressed = anyPressed(action.keys());
        if (action.pressed() != pressed)
        {
            action.pressed(pressed);
            CUBOS_INFO("Action {} was {}", boundAction.name, pressed ? "pressed" : "released");
        }
    }

    for (const auto& boundAxis : mBoundAxes[event.key])
    {
        auto& axis = mBindings[boundAxis.player].axes()[boundAxis.name];
        if (boundAxis.negative)
        {
            handleKeys(axis.negative(), event);
        }
        else
        {
            handleKeys(axis.positive(), event);
        }

        float value = 0.0f;
        if (anyPressed(axis.positive()))
            value += 1.0f;
        if (anyPressed(axis.negative()))
            value -= 1.0f;

        if (axis.value() != value)
        {
            axis.value(value);
            CUBOS_INFO("Axis {} value is {}", boundAxis.name, value);
        }
    }
}

void Input::handle(const ModifiersEvent& event)
{
    mModifiers = event.modifiers;
}

const std::unordered_map<int, InputBindings>& Input::bindings() const
{
    return mBindings;
}