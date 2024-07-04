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

        ~InputAction() = default;

        /// @brief Constructs without any bindings.
        InputAction() = default;

        /// @brief Constructs with existing bindings.
        /// @param combinations Combinations to bind.
        InputAction(std::vector<InputCombination> combinations)
            : mCombinations(std::move(combinations))
        {
        }

        /// @brief Gets the bound combinations.
        /// @return Vector of combinations.
        const std::vector<InputCombination>& combinations() const;

        /// @brief Gets the bound combinations.
        /// @return Vector of combinations.
        std::vector<InputCombination>& combinations();

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

        /// @brief Checks if this action was just released.
        /// @param justReleased Whether this action was just released.
        void justReleased(bool justReleased);

    private:
        std::vector<InputCombination> mCombinations;

        bool mPressed;      ///< Not serialized.
        bool mJustPressed;  ///< Not serialized.
        bool mJustReleased; ///< Not serialized;
    };
} // namespace cubos::engine
