/// @file
/// @brief

#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <cubos/core/io/keyboard.hpp>

namespace cubos::engine
{
    class KeyWithModifiers final
    {
    public:
        KeyWithModifiers() = default;
        KeyWithModifiers(core::io::Key key, core::io::Modifiers modifiers)
            : mKey(key)
            , mModifiers(modifiers)
        {
        }

        ~KeyWithModifiers() = default;

        const core::io::Key& key() const;
        const core::io::Modifiers& modifiers() const;

        core::io::Key& key();
        core::io::Modifiers& modifiers();

        std::string toString() const;

    private:
        core::io::Key mKey = core::io::Key::Invalid;
        core::io::Modifiers mModifiers = core::io::Modifiers::None;
    };

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

    class InputBindings final
    {
    public:
        InputBindings() = default;
        ~InputBindings() = default;

        const std::unordered_map<std::string, InputAction>& actions() const;
        const std::unordered_map<std::string, InputAxis>& axes() const;

        std::unordered_map<std::string, InputAction>& actions();
        std::unordered_map<std::string, InputAxis>& axes();

        std::string toString() const;

    private:
        std::unordered_map<std::string, InputAction> mActions;
        std::unordered_map<std::string, InputAxis> mAxes;
    };
} // namespace cubos::engine