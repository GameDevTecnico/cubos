/// @file
/// @brief Resource @ref cubos::engine::InputChangeTracker.
/// @ingroup input-plugin

#pragma once

#include <bitset>

#include <cubos/core/io/gamepad.hpp>
#include <cubos/core/io/keyboard.hpp>
#include <cubos/core/io/window.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Resource that tracks which inputs require attention.
    ///
    /// Its state is updated accordingly as events are received by the @ref input-plugin.
    ///
    /// @ingroup input-plugin
    class CUBOS_ENGINE_API InputEventTracker final
    {
    public:
        constexpr static auto KeyCount = static_cast<size_t>(core::io::Key::Count);
        constexpr static auto ButtonCount =
            static_cast<size_t>(core::io::MouseButton::Count) + static_cast<size_t>(core::io::GamepadButton::Count);
        constexpr static auto AxisCount =
            static_cast<size_t>(core::io::GamepadButton::Count) + static_cast<size_t>(core::io::GamepadAxis::Count);

        InputEventTracker() = default;
        ~InputEventTracker() = default;

        void listen(core::io::Key key);
        void listen(core::io::MouseButton button);
        void listen(core::io::GamepadButton button);
        void listen(core::io::MouseAxis axis);
        void listen(core::io::GamepadAxis axis);

        void ignore(core::io::Key key);
        void ignore(core::io::MouseButton button);
        void ignore(core::io::GamepadButton button);
        void ignore(core::io::MouseAxis axis);
        void ignore(core::io::GamepadAxis axis);

        bool trigger(core::io::Key key);
        bool trigger(core::io::MouseButton button);
        bool trigger(core::io::GamepadButton button);
        bool trigger(core::io::MouseAxis axis);
        bool trigger(core::io::GamepadAxis axis);

        const std::bitset<KeyCount>& keysTriggered() const;
        const std::bitset<ButtonCount>& buttonsTriggered() const;
        const std::bitset<AxisCount>& axesTriggered() const;

    private:
        std::bitset<KeyCount> mKeysListening;
        std::bitset<KeyCount> mKeysTriggered;

        std::bitset<ButtonCount> mButtonsListening;
        std::bitset<ButtonCount> mButtonsTriggered;

        std::bitset<AxisCount> mAxesListening;
        std::bitset<AxisCount> mAxesTriggered;
    };
} // namespace cubos::engine
