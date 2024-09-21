/// @file
/// @brief Class @ref cubos::engine::InputAction.
/// @ingroup input-plugin

#pragma once

#include <vector>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/input/combination.hpp>

namespace cubos::engine
{
    /// @brief Stores the state of a single input action, such as "jump" or "attack".
    ///
    /// Can be bound to multiple key combinations, and will be considered "pressed" if any of them are pressed.
    ///
    /// @ingroup input-plugin
    class CUBOS_ENGINE_API InputAction final
    {
    public:
        CUBOS_REFLECT;

        /// @brief Updates the state of the action.
        /// @param window Window to get the value from.
        void update(const core::io::Window& window);

        /// @brief Checks if this action is pressed.
        /// @return Sets whether this action is pressed.
        bool pressed() const;

        /// @brief Checks if this action was just pressed.
        /// @return Sets whether this action was just pressed.
        bool justPressed() const;

        /// @brief Checks if this action was just released.
        /// @return Sets whether this action was just released.
        bool justReleased() const;

        std::vector<InputCombination> combinations;

    private:
        bool mPressed;
        bool mJustPressed;
        bool mJustReleased;
    };

    static const InputAction NullAction = InputAction{};
} // namespace cubos::engine
