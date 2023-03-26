#pragma once

#include <list>
#include <map>
#include <memory>
#include <string>
#include <variant>

#include <glm/glm.hpp>

#include <cubos/core/event.hpp>
#include <cubos/core/io/keyboard.hpp>
#include <cubos/core/io/sources/source.hpp>
#include <cubos/core/io/window.hpp>

namespace cubos::core::io
{
    /// DoubleAxis is used to bind position change in two axis to the gameplay logic.
    /// The bindings are only invoken when change in the position occurs.
    /// Handles events subscription and context creation for when a change in the position on two axis.
    /// Handles mouse axis
    /// @see Source
    class DoubleAxis : public Source
    {
    public:
        /// Creates a Double Axis Source associated to two mouse axis
        /// @param horizontalAxis the horizontal axis assoaiated to this Double Axis source
        /// @param verticalAxis the vertical axis assoaiated to this Double Axis source
        DoubleAxis(cubos::core::io::MouseAxis horizontalAxis, cubos::core::io::MouseAxis verticalAxis);

        /// Checks if the position in the axis associated with this Double Axis source has been changed since last
        /// checked
        /// @return true if the position associated with this Double Axis has been changed, otherwise false
        bool isTriggered() override;

        /// Subscribes this object to callbacks in the Input Manager
        /// @see unsubscribeEvents
        void subscribeEvents() override;

        /// Unsubscribes this object to callbacks in the Input Manager
        /// @see subscribeEvents
        void unsubscribeEvents() override;

        /// Creates the context related to this Double Axis
        /// @return context created by this Double Axis
        Context createContext() override;

    private:
        bool wasTriggered = false;
        float xPos;
        float yPos;
        using Axis = std::variant<cubos::core::io::MouseAxis>;
        Axis horizontalAxis;
        Axis verticalAxis;
        void handleHorizontalAxis(float xPos);
        void handleVerticalAxis(float yPos);
    };
} // namespace cubos::core::io
