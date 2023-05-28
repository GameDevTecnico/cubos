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
        KeyWithModifier(cubos::core::io::Key key, cubos::core::io::Modifiers modifiers)
            : mKey(key)
            , mModifiers(modifiers)
        {
        }

        ~KeyWithModifier() = default;

        const cubos::core::io::Key& getKey() const;
        const cubos::core::io::Modifiers& getModifiers() const;

        cubos::core::io::Key& getKey();
        cubos::core::io::Modifiers& getModifiers();

        std::string toString() const;

    private:
        cubos::core::io::Key mKey = cubos::core::io::Key::Invalid;
        cubos::core::io::Modifiers mModifiers = cubos::core::io::Modifiers::None;
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

        const std::vector<KeyWithModifier>& getPositive() const;
        const std::vector<KeyWithModifier>& getNegative() const;

        std::vector<KeyWithModifier>& getPositive();
        std::vector<KeyWithModifier>& getNegative();

        float getValue() const;
        void setValue(float value);

        std::string toString() const;

    private:
        std::vector<KeyWithModifier> mPositive;
        std::vector<KeyWithModifier> mNegative;

        // Not serialized
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

        const std::vector<KeyWithModifier>& getKeys() const;
        std::vector<KeyWithModifier>& getKeys();

        bool isPressed() const;
        void setPressed(bool pressed);

        std::string toString() const;

    private:
        std::vector<KeyWithModifier> mKeys;

        // Not serialized
        bool mPressed;
    };

    class InputBindings final
    {
    public:
        InputBindings() = default;
        ~InputBindings() = default;

        const std::unordered_map<std::string, InputAction>& getActions() const;
        const std::unordered_map<std::string, InputAxis>& getAxes() const;

        std::unordered_map<std::string, InputAction>& getActions();
        std::unordered_map<std::string, InputAxis>& getAxes();

        std::string toString() const;

    private:
        std::unordered_map<std::string, InputAction> mActions;
        std::unordered_map<std::string, InputAxis> mAxes;
    };
} // namespace cubos::engine