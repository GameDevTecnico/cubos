#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>

#include <cubos/engine/input/input.hpp>

using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(Input)
{
    return core::ecs::TypeBuilder<Input>("cubos::engine::Input").withField("bindings", &Input::mBindings).build();
}

static void handle(auto&& /*unused*/)
{
}

void Input::handle(const Window& window, const WindowEvent& event)
{
    std::visit([&, window](const auto& e) { this->handle(window, e); }, event);
}

void Input::update(const Window& window)
{
    for (auto& [player, bindings] : mBindings)
    {
        for (auto& [_, action] : bindings.actions())
        {
            action.update(window);
        }

        for (auto& [_, axis] : bindings.axes())
        {
            axis.update(window);
        }
    }
}

void Input::clear()
{
    mBindings.clear();
    // mEventTrackers.clear();
    CUBOS_DEBUG("Input bindings cleared");
}

void Input::clear(int player)
{
    mBindings.erase(player);
    // mEventTrackers.erase(player);
    CUBOS_DEBUG("Input bindings cleared for player {}", player);
}

void Input::bind(const InputBindings& bindings, int player)
{
    this->clear(player);

    // for (const auto& [actionName, action] : bindings.actions())
    // {
    //     action.track(mEventTrackers[player]);
    // }

    // for (const auto& [axisName, axis] : bindings.axes())
    // {
    //     axis.track(mEventTrackers[player]);
    // }

    mBindings[player] = bindings;
    CUBOS_DEBUG("Input bindings set for player {}", player);
}

void Input::bind(const char* actionName, const InputAction& action, int player)
{
    mBindings[player].actions()[actionName] = action;

    // action.track(mEventTrackers[player]);

    CUBOS_DEBUG("Action {} bound to player {}", actionName, player);
}

void Input::bind(const char* axisName, const InputAxis& axis, int player)
{
    mBindings[player].axes()[axisName] = axis;

    // action.track(mEventTrackers[player]);

    CUBOS_DEBUG("Axis {} bound to player {}", axisName, player);
}

const InputAction& Input::action(const char* actionName, int player) const
{
    auto pIt = mBindings.find(player);
    if (pIt == mBindings.end())
    {
        CUBOS_WARN("Player {} does not have any associated input bindings", player);
        return NullAction;
    }

    auto aIt = pIt->second.actions().find(actionName);
    if (aIt == pIt->second.actions().end())
    {
        CUBOS_WARN("Action {} is not bound to any input for player {}", actionName, player);
        return NullAction;
    }

    return aIt->second;
}

const InputAxis& Input::axis(const char* axisName, int player) const
{
    auto pIt = mBindings.find(player);
    if (pIt == mBindings.end())
    {
        CUBOS_WARN("Player {} does not have any associated input bindings", player);
        return NullAxis;
    }

    auto aIt = pIt->second.axes().find(axisName);
    if (aIt == pIt->second.axes().end())
    {
        CUBOS_WARN("Axis {} is not bound to any input for player {}", axisName, player);
        return NullAxis;
    }

    return aIt->second;
}

const std::unordered_map<int, InputBindings>& Input::bindings() const
{
    return mBindings;
}

// bool Input::pressed(const char* actionName, int player) const
// {
//     auto pIt = mPlayerBindings.find(player);
//     if (pIt == mPlayerBindings.end())
//     {
//         CUBOS_WARN("Player {} does not have any associated input bindings", player);
//         return false;
//     }

//     auto aIt = pIt->second.actions().find(actionName);
//     if (aIt == pIt->second.actions().end())
//     {
//         CUBOS_WARN("Action {} is not bound to any input for player {}", actionName, player);
//         return false;
//     }

//     return aIt->second.pressed();
// }

// bool Input::justPressed(const char* actionName, int player) const
// {
//     auto pIt = mPlayerBindings.find(player);
//     if (pIt == mPlayerBindings.end())
//     {
//         CUBOS_WARN("Player {} does not have any associated input bindings", player);
//         return false;
//     }

//     auto aIt = pIt->second.actions().find(actionName);
//     if (aIt == pIt->second.actions().end())
//     {
//         CUBOS_WARN("Action {} is not bound to any input for player {}", actionName, player);
//         return false;
//     }

//     return aIt->second.justPressed();
// }

// bool Input::justReleased(const char* actionName, int player) const
// {
//     auto pIt = mPlayerBindings.find(player);
//     if (pIt == mPlayerBindings.end())
//     {
//         CUBOS_WARN("Player {} does not have any associated input bindings", player);
//         return false;
//     }

//     auto aIt = pIt->second.actions().find(actionName);
//     if (aIt == pIt->second.actions().end())
//     {
//         CUBOS_WARN("Action {} is not bound to any input for player {}", actionName, player);
//         return false;
//     }

//     return aIt->second.justReleased();
// }

// float Input::axis(const char* axisName, int player) const
// {
//     auto pIt = mPlayerBindings.find(player);
//     if (pIt == mPlayerBindings.end())
//     {
//         CUBOS_WARN("Player {} does not have any associated input bindings", player);
//         return 0.0F;
//     }

//     auto aIt = pIt->second.axes().find(axisName);
//     if (aIt == pIt->second.axes().end())
//     {
//         CUBOS_WARN("Axis {} is not bound to any input for player {}", axisName, player);
//         return 0.0F;
//     }

//     return aIt->second.value();
// }

// void Input::handleActions(const Window& window, const std::vector<BindingIndex>& boundActions)
// {
//     for (const auto& boundAction : boundActions)
//     {
//         auto& action = mPlayerBindings[boundAction.player].actions()[boundAction.name];
//         auto pressed = anyPressed(boundAction.player, window, action.combinations());

//         if (action.pressed() != pressed)
//         {
//             action.pressed(pressed);
//             CUBOS_TRACE("Action {} was {}", boundAction.name, pressed ? "pressed" : "released");

//             if (pressed)
//             {
//                 action.justPressed(true);
//             }
//             else
//             {
//                 action.justReleased(true);
//             }
//         }
//     }
// }

// void Input::handleAxes(const Window& window, const std::vector<BindingIndex>& boundAxes)
// {
//     for (const auto& boundAxis : boundAxes)
//     {
//         auto& axis = mPlayerBindings[boundAxis.player].axes()[boundAxis.name];

//         float value = 0.0F;
//         if (anyPressed(boundAxis.player, window, axis.negative()))
//         {
//             value -= 1.0F;
//         }
//         if (anyPressed(boundAxis.player, window, axis.positive()))
//         {
//             value += 1.0F;
//         }
//         if (auto it = mPlayerGamepads.find(boundAxis.player); it != mPlayerGamepads.end())
//         {
//             for (auto gamepadAxis : axis.gamepadAxes())
//             {
//                 value += mGamepadStates[it->second].axis(gamepadAxis);
//             }
//         }

//         if (axis.value() != value)
//         {
//             axis.value(value);
//             CUBOS_TRACE("Axis {} value is {}", boundAxis.name, value);
//         }
//     }
// }

// void Input::handle(const Window& window, const KeyEvent& event)
// {
//     this->handleActions(window, mBoundKeyActions[event.key]);
//     this->handleAxes(window, mBoundKeyAxes[event.key]);
// }

// void Input::handle(const Window& /*unused*/, const GamepadConnectionEvent& event)
// {
//     if (event.connected)
//     {
//         mGamepadStates[event.gamepad] = GamepadState{};
//         if (!mPlayerGamepads.contains(0))
//         {
//             this->gamepad(0, event.gamepad);
//         }
//         else if (!mPlayerGamepads.contains(1))
//         {
//             this->gamepad(1, event.gamepad);
//         }
//         CUBOS_DEBUG("Gamepad {} connected", event.gamepad);
//     }
//     else
//     {
//         mGamepadStates.erase(event.gamepad);
//         for (const auto& playerGamepad : mPlayerGamepads)
//         {
//             if (playerGamepad.second == event.gamepad)
//             {
//                 this->gamepad(playerGamepad.first, -1);
//                 break;
//             }
//         }
//         CUBOS_DEBUG("Gamepad {} disconnected", event.gamepad);
//     }
// }

// void Input::handle(const Window& window, const MouseButtonEvent& event)
// {
//     this->handleActions(window, mBoundMouseButtonActions[event.button]);
// }

// void Input::handle(const Window& /*unused*/, const MouseMoveEvent& event)
// {
//     mMousePosition = event.position;
// }

// void Input::updateMouse()
// {
//     mPreviousMousePosition = mMousePosition;
// }

// void Input::updateActions()
// {
//     for (auto& [_, bindings] : mPlayerBindings)
//     {
//         for (auto& [_, action] : bindings.actions())
//         {
//             action.justPressed(false);
//             action.justReleased(false);
//         }
//     }
// }

// glm::ivec2 Input::mousePosition() const
// {
//     return mMousePosition;
// }

// glm::ivec2 Input::previousMousePosition() const
// {
//     return mPreviousMousePosition;
// }

// glm::ivec2 Input::mouseDelta() const
// {
//     return mMousePosition - mPreviousMousePosition;
// }

// void Input::pollGamepads(const Window& window)
// {
//     for (auto& [gamepad, state] : mGamepadStates)
//     {
//         // Get the new state of the gamepad.
//         GamepadState oldState = state;
//         if (!window->gamepadState(gamepad, state))
//         {
//             CUBOS_WARN("Failed to get state for gamepad {}", gamepad);
//         }

//         // Check if any buttons were pressed or released.
//         for (int i = 0; i < static_cast<int>(GamepadButton::Count); ++i)
//         {
//             if (state.buttons[i] != oldState.buttons[i])
//             {
//                 this->handleActions(window, mBoundGamepadButtonActions[static_cast<GamepadButton>(i)]);
//             }
//         }

//         // Check if any of the axes changed.
//         for (int i = 0; i < static_cast<int>(GamepadAxis::Count); ++i)
//         {
//             if (state.axes[i] != oldState.axes[i])
//             {
//                 this->handleAxes(window, mBoundGamepadAxes[static_cast<GamepadAxis>(i)]);
//             }
//         }
//     }
// }

// const std::unordered_map<int, InputBindings>& Input::bindings() const
// {
//     return mPlayerBindings;
// }
