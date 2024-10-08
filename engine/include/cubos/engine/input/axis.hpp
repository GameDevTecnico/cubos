/// @file
/// @brief Class @ref cubos::engine::InputAxis.
/// @ingroup input-plugin

#pragma once

#include <vector>

#include <cubos/core/io/gamepad.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/input/combination.hpp>

namespace cubos::engine
{
    /// @brief Stores the state of a single input axis, such as "move forward" or "move right".
    ///
    /// Can be bound to multiple keys, and will have a value in the range [-1, 1] based on the the
    /// state of its bindings.
    ///
    /// @ingroup input-plugin
    class CUBOS_ENGINE_API InputAxis final
    {
    public:
        CUBOS_REFLECT;

        ~InputAxis() = default;

        /// @brief Constructs without any bindings.
        InputAxis() = default;

        /// @brief Constructs with existing bindings.
        /// @param positive Positive input combinations.
        /// @param negative Negative input combinations.
        /// @param gamepadAxes Gamepad axis bindings.
        InputAxis(std::vector<InputCombination> positive, std::vector<InputCombination> negative,
                  std::vector<core::io::GamepadAxis> gamepadAxes, float deadzone = 0.0F)
            : mPositive(std::move(positive))
            , mNegative(std::move(negative))
            , mGamepadAxes(std::move(gamepadAxes))
            , mDeadzone(deadzone)
        {
        }

        /// @brief Gets the positive input combinations.
        /// @return Vector of positive input combinations.
        const std::vector<InputCombination>& positive() const;

        /// @brief Gets the negative input combinations.
        /// @return Vector of negative input combinations.
        const std::vector<InputCombination>& negative() const;

        /// @brief Gets the gamepad axis bindings.
        /// @return Vector of gamepad axes.
        const std::vector<core::io::GamepadAxis>& gamepadAxes() const;

        /// @brief Gets the positive input combinations.
        /// @return Vector of positive input combinations.
        std::vector<InputCombination>& positive();

        /// @brief Gets the negative input combinations.
        /// @return Vector of negative input combinations.
        std::vector<InputCombination>& negative();

        /// @brief Gets the gamepad axis bindings.
        /// @return Vector of gamepad axes.
        std::vector<core::io::GamepadAxis>& gamepadAxes();

        /// @brief Gets the value.
        /// @return Value.
        float value() const;

        /// @brief Sets the value.
        /// @param value New value.
        void value(float value);

        /// @brief Gets the deadzone.
        /// @return Deadzone.
        float deadzone() const;

        /// @brief Sets the deadzone.
        /// @param deadzone New deadzone.
        void deadzone(float deadzone);

    private:
        std::vector<InputCombination> mPositive;
        std::vector<InputCombination> mNegative;
        std::vector<core::io::GamepadAxis> mGamepadAxes;

        float mValue{0.0F}; ///< Not serialized.
        float mDeadzone{0.0F};
    };
} // namespace cubos::engine
