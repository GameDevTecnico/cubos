#include <cubos/core/data/debug_serializer.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/input/input.hpp>

using cubos::core::data::Debug;
using cubos::core::io::KeyEvent;
using cubos::core::io::Window;
using namespace cubos::engine;

void Input::clear()
{
    mBoundActions.clear();
    mBoundAxes.clear();
    mBindings.clear();
    CUBOS_DEBUG("Input bindings cleared");
}

void Input::clear(int player)
{
    for (const auto& action : mBindings[player].actions())
    {
        for (const auto& key : action.second.keys())
        {
            std::erase_if(mBoundActions[key.first], [player](const auto& idx) { return idx.player == player; });
        }
    }

    for (const auto& axis : mBindings[player].axes())
    {
        for (const auto& pos : axis.second.positive())
        {
            std::erase_if(mBoundAxes[pos.first], [player](const auto& idx) { return idx.player == player; });
        }

        for (const auto& neg : axis.second.negative())
        {
            std::erase_if(mBoundAxes[neg.first], [player](const auto& idx) { return idx.player == player; });
        }
    }

    mBindings.erase(player);
    CUBOS_DEBUG("Input bindings cleared for player {}", player);
}

void Input::bind(const InputBindings& bindings, int player)
{
    this->clear(player);

    for (const auto& action : bindings.actions())
    {
        for (const auto& key : action.second.keys())
        {
            mBoundActions[key.first].push_back(BindingIndex{action.first, player});
        }
    }

    for (const auto& axis : bindings.axes())
    {
        for (const auto& pos : axis.second.positive())
        {
            mBoundAxes[pos.first].push_back(BindingIndex{axis.first, player, false});
        }

        for (const auto& neg : axis.second.negative())
        {
            mBoundAxes[neg.first].push_back(BindingIndex{axis.first, player, true});
        }
    }

    mBindings[player] = bindings;
    CUBOS_DEBUG("Input bindings set for player {}", player);
}

bool Input::pressed(const char* actionName, int player) const
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

float Input::axis(const char* axisName, int player) const
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

bool Input::anyPressed(const Window& window, const std::vector<std::pair<Key, Modifiers>>& keys) const
{
    for (auto& key : keys)
    {
        if (window->keyPressed(key.first, key.second))
        {
            return true;
        }
    }
    return false;
}

void Input::handle(const Window& window, const KeyEvent& event)
{
    for (const auto& boundAction : mBoundActions[event.key])
    {
        auto& action = mBindings[boundAction.player].actions()[boundAction.name];

        if (auto pressed = anyPressed(window, action.keys()); action.pressed() != pressed)
        {
            action.pressed(pressed);
            CUBOS_TRACE("Action {} was {}", boundAction.name, pressed ? "pressed" : "released");
        }
    }

    for (const auto& boundAxis : mBoundAxes[event.key])
    {
        auto& axis = mBindings[boundAxis.player].axes()[boundAxis.name];

        float value = 0.0f;
        if (anyPressed(window, axis.negative()))
        {
            value -= 1.0f;
        }
        if (anyPressed(window, axis.positive()))
        {
            value += 1.0f;
        }

        if (axis.value() != value)
        {
            axis.value(value);
            CUBOS_TRACE("Axis {} value is {}", boundAxis.name, value);
        }
    }
}

const std::unordered_map<int, InputBindings>& Input::bindings() const
{
    return mBindings;
}
