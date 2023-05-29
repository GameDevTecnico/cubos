#include <cubos/core/log.hpp>

#include <cubos/engine/input/input.hpp>

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
        CUBOS_WARN("Player {} does not exist", player);
        return false;
    }

    auto aIt = pIt->second.actions().find(actionName);
    if (aIt == pIt->second.actions().end())
    {
        CUBOS_WARN("Action {} does not exist", actionName);
        return false;
    }

    return aIt->second.pressed();
}

float Input::axis(const std::string& axisName, int player) const
{
    auto pIt = mBindings.find(player);
    if (pIt == mBindings.end())
    {
        CUBOS_WARN("Player {} does not exist", player);
        return 0.0f;
    }

    auto aIt = pIt->second.axes().find(axisName);
    if (aIt == pIt->second.axes().end())
    {
        CUBOS_WARN("Axis {} does not exist", axisName);
        return 0.0f;
    }

    return aIt->second.value();
}

const std::unordered_map<int, InputBindings>& Input::bindings() const
{
    return mBindings;
}