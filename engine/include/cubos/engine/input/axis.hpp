/// @file
/// @brief Contains the InputAxis class.
#pragma once

#include <vector>

#include <cubos/engine/input/key_with_modifiers.hpp>

namespace cubos::engine
{
    /// Used to represent a single input axis, such as "move forward" or "move right".
    /// It can be bound to multiple keys, and will return a value in the range [-1, 1] based on which keys are pressed.
    class InputAxis final
    {
    public:
        InputAxis() = default;
        InputAxis(std::vector<KeyWithModifiers> positive, std::vector<KeyWithModifiers> negative)
            : mPositive(positive)
            , mNegative(negative)
        {
        }

        ~InputAxis() = default;

        /// @return The vector of positive keys.
        const std::vector<KeyWithModifiers>& positive() const;

        /// @return The vector of negative keys.
        const std::vector<KeyWithModifiers>& negative() const;

        /// @return The vector of positive keys.
        std::vector<KeyWithModifiers>& positive();

        /// @return The vector of negative keys.
        std::vector<KeyWithModifiers>& negative();

        /// @return The value of this axis.
        float value() const;

        /// Sets the value of this axis.
        /// @param value The new value of this axis.
        void value(float value);

    private:
        std::vector<KeyWithModifiers> mPositive;
        std::vector<KeyWithModifiers> mNegative;

        /// Not serialized.
        float mValue;
    };
} // namespace cubos::engine
