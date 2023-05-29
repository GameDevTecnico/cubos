#pragma once

#include <vector>

#include <cubos/engine/input/key_with_modifiers.hpp>

namespace cubos::engine
{
    class InputAction final
    {
    public:
        InputAction() = default;
        InputAction(std::vector<KeyWithModifiers> keys)
            : mKeys(keys)
        {
        }

        ~InputAction() = default;

        const std::vector<KeyWithModifiers>& keys() const;
        std::vector<KeyWithModifiers>& keys();

        bool pressed() const;
        void pressed(bool pressed);

        std::string toString() const;

    private:
        std::vector<KeyWithModifiers> mKeys;

        /// Not serialized.
        bool mPressed;
    };
} // namespace cubos::engine