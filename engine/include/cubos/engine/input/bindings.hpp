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
    class KeyWithModifier final
    {
    public:
        KeyWithModifier() = default;
        KeyWithModifier(core::io::Key key, core::io::Modifiers modifiers)
            : mKey(key)
            , mModifiers(modifiers)
        {
        }

        ~KeyWithModifier() = default;

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
        InputAxis(std::vector<KeyWithModifier> positive, std::vector<KeyWithModifier> negative)
            : mPositive(positive)
            , mNegative(negative)
        {
        }

        ~InputAxis() = default;

        const std::vector<KeyWithModifier>& positive() const;
        const std::vector<KeyWithModifier>& negative() const;

        std::vector<KeyWithModifier>& positive();
        std::vector<KeyWithModifier>& negative();

        float value() const;
        void value(float value);

        std::string toString() const;

    private:
        std::vector<KeyWithModifier> mPositive;
        std::vector<KeyWithModifier> mNegative;

        /// Not serialized.
        float mValue;
    };

    class InputAction final
    {
    public:
        InputAction() = default;
        InputAction(std::vector<KeyWithModifier> keys)
            : mKeys(keys)
        {
        }

        ~InputAction() = default;

        const std::vector<KeyWithModifier>& keys() const;
        std::vector<KeyWithModifier>& keys();

        bool pressed() const;
        void pressed(bool pressed);

        std::string toString() const;

    private:
        std::vector<KeyWithModifier> mKeys;

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