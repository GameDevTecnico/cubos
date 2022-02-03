#ifndef CUBOS_IO_DOUBLE_AXIS_HPP
#define CUBOS_IO_DOUBLE_AXIS_HPP

#include <cubos/event.hpp>
#include <cubos/io/window.hpp>
#include <cubos/io/keyboard.hpp>
#include <map>
#include <string>
#include <memory>
#include <variant>
#include <list>
#include <glm/glm.hpp>
#include <cubos/io/sources/source.hpp>

namespace cubos::io
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
        DoubleAxis(cubos::io::MouseAxis horizontalAxis, cubos::io::MouseAxis verticalAxis);

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
        using Axis = std::variant<cubos::io::MouseAxis>;
        Axis horizontalAxis;
        Axis verticalAxis;
        void handleHorizontalAxis(float xPos);
        void handleVerticalAxis(float yPos);
    };
} // namespace cubos::io

#endif // CUBOS_IO_DOUBLE_AXIS_HPP