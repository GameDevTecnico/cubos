/// @file
/// @brief Class @ref cubos::engine::InputAction.
/// @ingroup input-plugin

#pragma once

#include <vector>

#include <cubos/core/io/gamepad.hpp>
#include <cubos/core/io/keyboard.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Stores the state of a single input action, such as "jump" or "attack".
    ///
    /// Can be bound to multiple keys, and will be considered "pressed" if any of them are pressed.
    ///
    /// @ingroup input-plugin
    class CUBOS_ENGINE_API InputAction final
    {
    public:
        CUBOS_REFLECT;

        ~InputAction() = default;

        /// @brief Constructs without any bindings.
        InputAction() = default;

        /// @brief Constructs with existing bindings.
        /// @param keys Key bindings.
        /// @param gamepadButtons Gamepad button bindings.
        /// @param mouseButtons Mouse button bindings.
        InputAction(std::vector<core::io::KeyWithModifiers> keys, std::vector<core::io::GamepadButton> gamepadButtons,
                    std::vector<core::io::MouseButton> mouseButtons)
            : mKeys(std::move(keys))
            , mGamepadButtons(std::move(gamepadButtons))
            , mMouseButtons(std::move(mouseButtons))
        {
        }

        /// @brief Gets the key bindings.
        /// @return Vector of keys.
        const std::vector<core::io::KeyWithModifiers>& keys() const;

        /// @brief Gets the key bindings.
        /// @return Vector of keys.
        std::vector<core::io::KeyWithModifiers>& keys();

        /// @brief Gets the gamepad button bindings.
        /// @return Vector of buttons.
        const std::vector<core::io::GamepadButton>& gamepadButtons() const;

        /// @brief Gets the gamepad button bindings.
        /// @return Vector of buttons.
        std::vector<core::io::GamepadButton>& gamepadButtons();

        /// @brief Gets the mouse button bindings.
        /// @return Vector of buttons.
        const std::vector<core::io::MouseButton>& mouseButtons() const;

        /// @brief Gets the mouse button bindings.
        /// @return Vector of buttons.
        std::vector<core::io::MouseButton>& mouseButtons();

        /// @brief Checks if this action is pressed.
        /// @return Whether this action is pressed.
        bool pressed() const;

        /// @brief Checks if this action was just pressed.
        /// @return Whether this action was just pressed.
        bool justPressed() const;

        /// @brief Checks if this action was just released.
        /// @return Whether this action was just released.
        bool justReleased() const;

        /// @brief Sets whether this action is pressed.
        /// @param pressed New pressed state.
        void pressed(bool pressed);

        /// @brief Checks if this action was just pressed.
        /// @param justPressed Whether this action was just pressed.
        void justPressed(bool justPressed);

        /// @brief Checks if this accion was just released.
        /// @param justReleased Whether this action was just released.
        void justReleased(bool justReleased);

    private:
        std::vector<core::io::KeyWithModifiers> mKeys;
        std::vector<core::io::GamepadButton> mGamepadButtons;
        std::vector<core::io::MouseButton> mMouseButtons;

        bool mPressed;      ///< Not serialized.
        bool mJustPressed;  ///< Not serialized.
        bool mJustReleased; ///< Not serialized;
    };
} // namespace cubos::engine
