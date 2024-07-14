/// @file
/// @brief Class @ref cubos::engine::InputCombination.
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
    /// @brief Stores the keys, gamepad buttons, and mouse buttons of a single input combination.
    ///
    /// Composed of multiple keys and buttons, and will be considered "pressed" if all of them are pressed.
    ///
    /// @ingroup input-plugin
    class CUBOS_ENGINE_API InputCombination final
    {
    public:
        CUBOS_REFLECT;

        ~InputCombination() = default;

        /// @brief Constructs without any keys or buttons.
        InputCombination() = default;

        /// @brief Constructs from sets of keys, gamepad buttons, and mouse buttons.
        /// @param keys Keys to be pressed.
        /// @param gamepadButtons Gamepad buttons to be pressed.
        /// @param mouseButtons Mouse buttons to be pressed.
        InputCombination(std::vector<core::io::Key> keys, std::vector<core::io::GamepadButton> gamepadButtons = {},
                         std::vector<core::io::MouseButton> mouseButtons = {})
            : mKeys(std::move(keys))
            , mGamepadButtons(std::move(gamepadButtons))
            , mMouseButtons(std::move(mouseButtons))
        {
        }

        /// @brief Gets the keys to be pressed.
        /// @return Vector of keys.
        const std::vector<core::io::Key>& keys() const;

        /// @brief Gets the keys to be pressed.
        /// @return Vector of keys.
        std::vector<core::io::Key>& keys();

        /// @brief Gets the gamepad buttons to be pressed.
        /// @return Vector of buttons.
        const std::vector<core::io::GamepadButton>& gamepadButtons() const;

        /// @brief Gets the gamepad buttons to be pressed.
        /// @return Vector of buttons.
        std::vector<core::io::GamepadButton>& gamepadButtons();

        /// @brief Gets the mouse buttons to be pressed.
        /// @return Vector of buttons.
        const std::vector<core::io::MouseButton>& mouseButtons() const;

        /// @brief Gets the mouse buttons to be pressed.
        /// @return Vector of buttons.
        std::vector<core::io::MouseButton>& mouseButtons();

        /// @brief Checks if the key combination is pressed.
        /// @param window Window to check for key and mouse button presses.
        /// @param gamepad Gamepad to check for button presses.
        /// @return Whether the key combination is pressed
        bool pressed(const core::io::Window& window, const core::io::GamepadState* gamepad) const;

    private:
        std::vector<core::io::Key> mKeys;                     ///< Keys to be pressed.
        std::vector<core::io::GamepadButton> mGamepadButtons; ///< Gamepad buttons to be pressed.
        std::vector<core::io::MouseButton> mMouseButtons;     ///< Mouse buttons to be pressed.
    };

} // namespace cubos::engine