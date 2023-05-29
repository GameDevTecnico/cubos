/// @file
/// @brief Contains the InputBindings class.

#pragma once

#include <unordered_map>

#include <cubos/engine/input/action.hpp>
#include <cubos/engine/input/axis.hpp>

namespace cubos::engine
{
    /// Used to store the input bindings for a single player.
    /// It contains a map of input actions and a map of input axes.
    class InputBindings final
    {
    public:
        InputBindings() = default;
        ~InputBindings() = default;

        /// @return The input actions map.
        const std::unordered_map<std::string, InputAction>& actions() const;

        /// @return The input axes map.
        const std::unordered_map<std::string, InputAxis>& axes() const;

        /// @return The input actions map.
        std::unordered_map<std::string, InputAction>& actions();

        /// @return The input axes map.
        std::unordered_map<std::string, InputAxis>& axes();

    private:
        std::unordered_map<std::string, InputAction> mActions;
        std::unordered_map<std::string, InputAxis> mAxes;
    };
} // namespace cubos::engine
