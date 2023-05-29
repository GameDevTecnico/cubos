/// @file
/// @brief Contains the InputAxis class.

#pragma once

#include <vector>

#include <cubos/core/io/keyboard.hpp>

namespace cubos::engine
{
    /// Used to represent a single input axis, such as "move forward" or "move right".
    /// It can be bound to multiple keys, and will return a value in the range [-1, 1] based on which keys are pressed.
    class InputAxis final
    {
    public:
        InputAxis() = default;
        InputAxis(std::vector<std::pair<core::io::Key, core::io::Modifiers>> positive,
                  std::vector<std::pair<core::io::Key, core::io::Modifiers>> negative)
            : mPositive(positive)
            , mNegative(negative)
        {
        }

        ~InputAxis() = default;

        /// @return The vector of positive keys.
        const std::vector<std::pair<core::io::Key, core::io::Modifiers>>& positive() const;

        /// @return The vector of negative keys.
        const std::vector<std::pair<core::io::Key, core::io::Modifiers>>& negative() const;

        /// @return The vector of positive keys.
        std::vector<std::pair<core::io::Key, core::io::Modifiers>>& positive();

        /// @return The vector of negative keys.
        std::vector<std::pair<core::io::Key, core::io::Modifiers>>& negative();

        /// @return The value of this axis.
        float value() const;

        /// Sets the value of this axis.
        /// @param value The new value of this axis.
        void value(float value);

    private:
        std::vector<std::pair<core::io::Key, core::io::Modifiers>> mPositive;
        std::vector<std::pair<core::io::Key, core::io::Modifiers>> mNegative;

        /// Not serialized.
        float mValue;
    };
} // namespace cubos::engine
