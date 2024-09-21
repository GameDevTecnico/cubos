/// @file
/// @brief Struct @ref cubos::engine::InputAxisSource
/// @ingroup input-plugin

#include <variant>

#include <cubos/core/io/gamepad.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

#pragma once

namespace cubos::engine
{

    /// @brief Provides the state of a single input axis, from any source.
    ///
    /// @ingroup input-plugin
    struct CUBOS_ENGINE_API InputAxisSource
    {
        CUBOS_REFLECT;

        struct CUBOS_ENGINE_API Gamepad
        {
            CUBOS_REFLECT;

            /// @brief Gets the value of the axis.
            /// @param window Window to get the value from.
            /// @return Value of the axis.
            float value(const core::io::Window& window) const;

            int index = 0;
            core::io::GamepadAxis axis = core::io::GamepadAxis::LX;
            float deadzone = 0.0F;
        };

        class CUBOS_ENGINE_API Mouse final
        {
        public:
            CUBOS_REFLECT;

            /// @brief Gets the value of the axis.
            /// @param window Window to get the value from.
            /// @return Value of the axis.
            float value(const core::io::Window& window) const;

            core::io::MouseAxis axis = core::io::MouseAxis::X;
            bool relative = false;

        private:
            float mPreviousValue = 0.0F;
        };

        /// @brief Gets the value of the axis.
        /// @param window Window to get the value from.
        /// @return Value of the axis.
        float value(const core::io::Window& window) const;

        std::variant<Gamepad, Mouse> source = Gamepad{};
    };
} // namespace cubos::engine
