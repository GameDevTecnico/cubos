#ifndef CUBOS_CORE_IO_SINGLE_AXIS_HPP
#define CUBOS_CORE_IO_SINGLE_AXIS_HPP

#include <cubos/io/window.hpp>
#include <cubos/io/sources/source.hpp>

#include <variant>
#include <tuple>

namespace cubos::core::io
{
    /// SingleAxis is used to bind position change in one axis to the gameplay logic.
    /// The bindings are only invoken when change in the position occurs.
    /// Handles events subscription and context creation for when a change in the position in an axis.
    /// Handles mouse axis and keyboard keys
    /// @see Source
    class SingleAxis : public Source
    {

    public:
        /// Creates a Single Axis source associated to a mouse axis
        /// @param axis the axis assoaiated to this Single Axis source
        SingleAxis(cubos::core::io::MouseAxis axis);

        /// Creates a Single Axis source associated to keyboard keys
        /// @param negativeKey the keyboard key associated to moving in the negative direction in the axis
        /// @param positiveKey the keyboard key associated to moving in the positive direction in the axis
        SingleAxis(cubos::core::io::Key negativeKey, cubos::core::io::Key positiveKey);

        /// Checks if the position in the axis associated with this Single Axis source has been changed since last
        /// checked
        /// @return true if the position associated with this Single Axis has been changed, otherwise false
        bool isTriggered() override;

        /// Subscribes this object to callbacks in the Input Manager
        /// @see unsubscribeEvents
        void subscribeEvents() override;

        /// Unsubscribes this object to callbacks in the Input Manager
        /// @see subscribeEvents
        void unsubscribeEvents() override;

        /// Creates the context related to this Single Axis
        /// @return context created by this Single Axis
        Context createContext() override;

    private:
        std::variant<cubos::core::io::MouseAxis, std::tuple<cubos::core::io::Key, cubos::core::io::Key>> inputs;
        void handleAxis(float value);
        void handlePositive();
        void handleNegative();
        bool wasTriggered;
        float value;
    };
} // namespace cubos::core::io

#endif // CUBOS_CORE_IO_SINGLE_AXIS_HPP
