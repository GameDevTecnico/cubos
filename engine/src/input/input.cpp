#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/input/input.hpp>

using cubos::core::io::GamepadButton;
using cubos::core::io::GamepadConnectionEvent;
using cubos::core::io::GamepadState;
using cubos::core::io::KeyEvent;
using cubos::core::io::MouseButton;
using cubos::core::io::MouseButtonEvent;
using cubos::core::io::MouseMoveEvent;
using cubos::core::io::Window;

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(Input)
{
    return core::ecs::TypeBuilder<Input>("cubos::engine::Input").build();
}

void Input::clear()
{
    mBoundKeyActions.clear();
    mBoundGamepadButtonActions.clear();
    mBoundMouseButtonActions.clear();
    mBoundKeyAxes.clear();
    mBoundMouseButtonAxes.clear();
    mBoundGamepadButtonAxes.clear();
    mBoundGamepadAxes.clear();

    mPlayerBindings.clear();
    CUBOS_DEBUG("Input bindings cleared");
}

void Input::clear(int player)
{
    for (const auto& action : mPlayerBindings[player].actions())
    {
        for (const auto& combo : action.second.combinations())
        {
            for (const auto& key : combo.keys())
            {
                std::erase_if(mBoundKeyActions[key], [player](const auto& idx) { return idx.player == player; });
            }

            for (const auto& button : combo.gamepadButtons())
            {
                std::erase_if(mBoundGamepadButtonActions[button],
                              [player](const auto& idx) { return idx.player == player; });
            }

            for (const auto& button : combo.mouseButtons())
            {
                std::erase_if(mBoundMouseButtonActions[button],
                              [player](const auto& idx) { return idx.player == player; });
            }
        }
    }

    for (const auto& axis : mPlayerBindings[player].axes())
    {
        for (const auto& pos : axis.second.positive())
        {
            for (const auto& key : pos.keys())
            {
                std::erase_if(mBoundKeyAxes[key], [player](const auto& idx) { return idx.player == player; });
            }

            for (const auto& button : pos.mouseButtons())
            {
                std::erase_if(mBoundMouseButtonAxes[button],
                              [player](const auto& idx) { return idx.player == player; });
            }

            for (const auto& button : pos.gamepadButtons())
            {
                std::erase_if(mBoundGamepadButtonActions[button],
                              [player](const auto& idx) { return idx.player == player; });
            }
        }

        for (const auto& neg : axis.second.negative())
        {
            for (const auto& key : neg.keys())
            {
                std::erase_if(mBoundKeyAxes[key], [player](const auto& idx) { return idx.player == player; });
            }

            for (const auto& button : neg.mouseButtons())
            {
                std::erase_if(mBoundMouseButtonAxes[button],
                              [player](const auto& idx) { return idx.player == player; });
            }

            for (const auto& button : neg.gamepadButtons())
            {
                std::erase_if(mBoundGamepadButtonAxes[button],
                              [player](const auto& idx) { return idx.player == player; });
            }
        }

        for (const auto& gamepadAxis : axis.second.gamepadAxes())
        {
            std::erase_if(mBoundGamepadAxes[gamepadAxis], [player](const auto& idx) { return idx.player == player; });
        }
    }

    mPlayerBindings.erase(player);
    CUBOS_DEBUG("Input bindings cleared for player {}", player);
}

void Input::bind(const InputBindings& bindings, int player)
{
    this->clear(player);

    for (const auto& action : bindings.actions())
    {
        for (const auto& combo : action.second.combinations())
        {
            for (const auto& key : combo.keys())
            {
                mBoundKeyActions[key].push_back(BindingIndex{action.first, player});
            }

            for (const auto& button : combo.gamepadButtons())
            {
                mBoundGamepadButtonActions[button].push_back(BindingIndex{action.first, player});
            }

            for (const auto& button : combo.mouseButtons())
            {
                mBoundMouseButtonActions[button].push_back(BindingIndex{action.first, player});
            }
        }
    }

    for (const auto& axis : bindings.axes())
    {
        for (const auto& pos : axis.second.positive())
        {
            for (const auto& key : pos.keys())
            {
                mBoundKeyAxes[key].push_back(BindingIndex{axis.first, player, false});
            }

            for (const auto& button : pos.mouseButtons())
            {
                mBoundMouseButtonAxes[button].push_back(BindingIndex{axis.first, player});
            }

            for (const auto& button : pos.gamepadButtons())
            {
                mBoundGamepadButtonActions[button].push_back(BindingIndex{axis.first, player});
            }
        }

        for (const auto& neg : axis.second.negative())
        {
            for (const auto& key : neg.keys())
            {
                mBoundKeyAxes[key].push_back(BindingIndex{axis.first, player, true});
            }

            for (const auto& button : neg.mouseButtons())
            {
                mBoundMouseButtonAxes[button].push_back(BindingIndex{axis.first, player});
            }

            for (const auto& button : neg.gamepadButtons())
            {
                mBoundGamepadButtonAxes[button].push_back(BindingIndex{axis.first, player});
            }
        }

        for (const auto& gamepadAxis : axis.second.gamepadAxes())
        {
            mBoundGamepadAxes[gamepadAxis].push_back(BindingIndex{axis.first, player});
        }
    }

    mPlayerBindings[player] = bindings;
    CUBOS_DEBUG("Input bindings set for player {}", player);
}

void Input::gamepad(int player, int gamepad)
{
    if (gamepad == -1)
    {
        mPlayerGamepads.erase(player);
        CUBOS_DEBUG("Gamepad unassigned from player {}", player);
    }
    else
    {
        mPlayerGamepads[player] = gamepad;
        CUBOS_DEBUG("Gamepad {} assigned to player {}", gamepad, player);
    }
}

int Input::gamepad(int player) const
{
    if (auto it = mPlayerGamepads.find(player); it != mPlayerGamepads.end())
    {
        return it->second;
    }

    return -1;
}

int Input::gamepadCount() const
{
    return static_cast<int>(mGamepadStates.size());
}

bool Input::pressed(const char* actionName, int player) const
{
    auto pIt = mPlayerBindings.find(player);
    if (pIt == mPlayerBindings.end())
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

bool Input::justPressed(const char* actionName, int player) const
{
    auto pIt = mPlayerBindings.find(player);
    if (pIt == mPlayerBindings.end())
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

    return aIt->second.justPressed();
}

bool Input::justReleased(const char* actionName, int player) const
{
    auto pIt = mPlayerBindings.find(player);
    if (pIt == mPlayerBindings.end())
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

    return aIt->second.justReleased();
}

float Input::axis(const char* axisName, int player) const
{
    auto pIt = mPlayerBindings.find(player);
    if (pIt == mPlayerBindings.end())
    {
        CUBOS_WARN("Player {} does not have any associated input bindings", player);
        return 0.0F;
    }

    auto aIt = pIt->second.axes().find(axisName);
    if (aIt == pIt->second.axes().end())
    {
        CUBOS_WARN("Axis {} is not bound to any input for player {}", axisName, player);
        return 0.0F;
    }

    return aIt->second.value();
}

bool Input::anyPressed(int player, const Window& window, const std::vector<InputCombination>& combinations) const
{
    GamepadState state;
    auto it = mPlayerGamepads.find(player);
    if (it != mPlayerGamepads.end())
    {
        state = mGamepadStates.at(it->second);
    }

    for (const auto& combo : combinations)
    {
        if (combo.pressed(window, it != mPlayerGamepads.end() ? &state : nullptr))
        {
            return true;
        }
    }
    return false;
}

void Input::handleActions(const Window& window, const std::vector<BindingIndex>& boundActions)
{
    for (const auto& boundAction : boundActions)
    {
        auto& action = mPlayerBindings[boundAction.player].actions()[boundAction.name];
        auto pressed = anyPressed(boundAction.player, window, action.combinations());

        if (action.pressed() != pressed)
        {
            action.pressed(pressed);
            CUBOS_TRACE("Action {} was {}", boundAction.name, pressed ? "pressed" : "released");

            if (pressed)
            {
                action.justPressed(true);
            }
            else
            {
                action.justReleased(true);
            }
        }
    }
}

void Input::handleAxes(const Window& window, const std::vector<BindingIndex>& boundAxes)
{
    for (const auto& boundAxis : boundAxes)
    {
        auto& axis = mPlayerBindings[boundAxis.player].axes()[boundAxis.name];

        float value = 0.0F;
        if (anyPressed(boundAxis.player, window, axis.negative()))
        {
            value -= 1.0F;
        }
        if (anyPressed(boundAxis.player, window, axis.positive()))
        {
            value += 1.0F;
        }
        if (auto it = mPlayerGamepads.find(boundAxis.player); it != mPlayerGamepads.end())
        {
            for (auto gamepadAxis : axis.gamepadAxes())
            {
                value += mGamepadStates[it->second].axis(gamepadAxis);
            }
        }

        if (axis.value() != value)
        {
            axis.value(value);
            CUBOS_TRACE("Axis {} value is {}", boundAxis.name, value);
        }
    }
}

void Input::handle(const Window& window, const KeyEvent& event)
{
    this->handleActions(window, mBoundKeyActions[event.key]);
    this->handleAxes(window, mBoundKeyAxes[event.key]);
}

void Input::handle(const Window& /*unused*/, const GamepadConnectionEvent& event)
{
    if (event.connected)
    {
        mGamepadStates[event.gamepad] = GamepadState{};
        if (!mPlayerGamepads.contains(0))
        {
            this->gamepad(0, event.gamepad);
        }
        else if (!mPlayerGamepads.contains(1))
        {
            this->gamepad(1, event.gamepad);
        }
        CUBOS_DEBUG("Gamepad {} connected", event.gamepad);
    }
    else
    {
        mGamepadStates.erase(event.gamepad);
        for (const auto& playerGamepad : mPlayerGamepads)
        {
            if (playerGamepad.second == event.gamepad)
            {
                this->gamepad(playerGamepad.first, -1);
                break;
            }
        }
        CUBOS_DEBUG("Gamepad {} disconnected", event.gamepad);
    }
}

void Input::handle(const Window& window, const MouseButtonEvent& event)
{
    this->handleActions(window, mBoundMouseButtonActions[event.button]);
}

void Input::handle(const Window& /*unused*/, const MouseMoveEvent& event)
{
    mMousePosition = event.position;
}

void Input::updateMouse()
{
    mPreviousMousePosition = mMousePosition;
}

void Input::updateActions()
{
    for (auto& [_, bindings] : mPlayerBindings)
    {
        for (auto& [_, action] : bindings.actions())
        {
            action.justPressed(false);
            action.justReleased(false);
        }
    }
}

glm::ivec2 Input::mousePosition() const
{
    return mMousePosition;
}

glm::ivec2 Input::previousMousePosition() const
{
    return mPreviousMousePosition;
}

glm::ivec2 Input::mouseDelta() const
{
    return mMousePosition - mPreviousMousePosition;
}

void Input::pollGamepads(const Window& window)
{
    for (auto& [gamepad, state] : mGamepadStates)
    {
        // Get the new state of the gamepad.
        GamepadState oldState = state;
        if (!window->gamepadState(gamepad, state))
        {
            CUBOS_WARN("Failed to get state for gamepad {}", gamepad);
        }

        // Check if any buttons were pressed or released.
        for (int i = 0; i < static_cast<int>(GamepadButton::Count); ++i)
        {
            if (state.buttons[i] != oldState.buttons[i])
            {
                this->handleActions(window, mBoundGamepadButtonActions[static_cast<GamepadButton>(i)]);
            }
        }

        // Check if any of the axes changed.
        for (int i = 0; i < static_cast<int>(GamepadAxis::Count); ++i)
        {
            if (state.axes[i] != oldState.axes[i])
            {
                this->handleAxes(window, mBoundGamepadAxes[static_cast<GamepadAxis>(i)]);
            }
        }
    }
}

const std::unordered_map<int, InputBindings>& Input::bindings() const
{
    return mPlayerBindings;
}
