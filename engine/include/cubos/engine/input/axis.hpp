/// @file
#pragma once

#include <vector>

#include <cubos/engine/input/key_with_modifiers.hpp>

namespace cubos::engine
{
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

        const std::vector<KeyWithModifiers>& positive() const;
        const std::vector<KeyWithModifiers>& negative() const;

        std::vector<KeyWithModifiers>& positive();
        std::vector<KeyWithModifiers>& negative();

        float value() const;
        void value(float value);

        std::string toString() const;

    private:
        std::vector<KeyWithModifiers> mPositive;
        std::vector<KeyWithModifiers> mNegative;

        /// Not serialized.
        float mValue;
    };
} // namespace cubos::engine