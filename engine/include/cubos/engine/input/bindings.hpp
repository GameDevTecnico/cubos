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

    class Axis final
    {
    public:
        Axis() = default;
        Axis(std::vector<KeyWithModifier> positive, std::vector<KeyWithModifier> negative)
            : mPositive(positive)
            , mNegative(negative)
        {
        }

        ~Axis() = default;

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

    class Action final
    {
    public:
        Action() = default;
        Action(std::vector<KeyWithModifier> keys)
            : mKeys(keys)
        {
        }

        ~Action() = default;

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

    class Bindings final
    {
    public:
        Bindings() = default;
        ~Bindings() = default;

        const std::unordered_map<std::string, Action>& getActions() const;
        const std::unordered_map<std::string, Axis>& getAxes() const;

        std::unordered_map<std::string, Action>& getActions();
        std::unordered_map<std::string, Axis>& getAxes();

        std::string toString() const;

    private:
        std::unordered_map<std::string, Action> mActions;
        std::unordered_map<std::string, Axis> mAxes;
    };
} // namespace cubos::engine