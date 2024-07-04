/// @file
/// @brief Class @ref cubos::engine::InputAxis.
/// @ingroup input-plugin

#pragma once

#include <vector>

#include <cubos/core/io/gamepad.hpp>
#include <cubos/core/io/keyboard.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

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
        /// @param positive Positive key bindings.
        /// @param negative Negative key bindings.
        /// @param gamepadAxes Gamepad axis bindings.
        InputAxis(std::vector<core::io::Key> positive, std::vector<core::io::Key> negative,
                  std::vector<core::io::GamepadAxis> gamepadAxes)
            : mPositive(std::move(positive))
            , mNegative(std::move(negative))
            , mGamepadAxes(std::move(gamepadAxes))
        {
        }

        /// @brief Gets the positive key bindings.
        /// @return Vector of positive keys.
        const std::vector<core::io::Key>& positive() const;

        /// @brief Gets the negative key bindings.
        /// @return Vector of negative keys.
        const std::vector<core::io::Key>& negative() const;

        /// @brief Gets the gamepad axis bindings.
        /// @return Vector of gamepad axes.
        const std::vector<core::io::GamepadAxis>& gamepadAxes() const;

        /// @brief Gets the positive key bindings.
        /// @return Vector of positive keys.
        std::vector<core::io::Key>& positive();

        /// @brief Gets the negative key bindings.
        /// @return Vector of negative keys.
        std::vector<core::io::Key>& negative();

        /// @brief Gets the gamepad axis bindings.
        /// @return Vector of gamepad axes.
        std::vector<core::io::GamepadAxis>& gamepadAxes();

        /// @brief Gets the value.
        /// @return Value.
        float value() const;

        /// @brief Sets the value.
        /// @param value New value.
        void value(float value);

    private:
        std::vector<core::io::Key> mPositive;
        std::vector<core::io::Key> mNegative;
        std::vector<core::io::GamepadAxis> mGamepadAxes;

        float mValue{0.0F}; ///< Not serialized.
    };
} // namespace cubos::engine
