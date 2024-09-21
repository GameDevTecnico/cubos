/// @file
/// @brief Struct @ref cubos::engine::InputPressSource
/// @ingroup input-plugin

#include <variant>

#include <cubos/core/io/window.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/input/sources/axis.hpp>

#pragma once

namespace cubos::engine
{
    /// @brief Provides the state of a pressable input, from any source.
    ///
    /// @ingroup input-plugin
    struct CUBOS_ENGINE_API InputPressSource
    {
        CUBOS_REFLECT;

        struct CUBOS_ENGINE_API Key
        {
            CUBOS_REFLECT;

            /// @brief Gets the value of the key.
            /// @param window Window to get the value from.
            /// @return Value of the key.
            bool pressed(const core::io::Window& window) const;

            core::io::Key key = core::io::Key::Space;
        };

        struct CUBOS_ENGINE_API Mouse
        {
            CUBOS_REFLECT;

            /// @brief Gets the value of the mouse button.
            /// @param window Window to get the value from.
            /// @return Value of the button.
            bool pressed(const core::io::Window& window) const;

            core::io::MouseButton button = core::io::MouseButton::Left;
        };

        struct CUBOS_ENGINE_API Gamepad
        {
            CUBOS_REFLECT;

            /// @brief Gets the value of the gamepad button.
            /// @param window Window to get the value from.
            /// @return Value of the button.
            bool pressed(const core::io::Window& window) const;

            int index = 0;
            core::io::GamepadButton button = core::io::GamepadButton::A;
        };

        struct CUBOS_ENGINE_API AxisActivation
        {
            CUBOS_REFLECT;

            /// @brief Gets the value of the mouse button.
            /// @param window Window to get the value from.
            /// @return Value of the button.
            bool pressed(const core::io::Window& window) const;

            InputAxisSource axis = InputAxisSource{};
            float activationStart = 0.5F;
            float activationEnd = 1.0F;
        };

        /// @brief Gets the value of the pressable input.
        /// @param window Window to get the value from.
        /// @return Value of the pressable input.
        bool pressed(const core::io::Window& window) const;

        std::variant<Key, Mouse, Gamepad, AxisActivation> source = Key{};
    };
} // namespace cubos::engine
