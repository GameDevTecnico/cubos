/// @file
/// @brief Class @ref cubos::core::ecs::EventWriter.
/// @ingroup core-ecs-system

#pragma once

#include <cubos/core/ecs/system/event/pipe.hpp>

namespace cubos::core::ecs
{
    /// @brief System argument which allows the system to send events of type @p T to other
    /// systems.
    /// @see Systems can read sent events using @ref EventReader.
    /// @tparam T
    /// @ingroup core-ecs-system
    template <typename T>
    class EventWriter
    {
    public:
        /// @brief Constructs.
        /// @param pipe Event pipe to write events to.
        EventWriter(EventPipe<T>& pipe);

        /// @brief Sends the given @p event to the event pipe with the given @p mask.
        /// @param event Event.
        /// @param mask Mask.
        void push(T event, unsigned int mask = DEFAULT_PUSH_MASK);

    private:
        EventPipe<T>& mPipe;
    };

    // EventWriter implementation.

    template <typename T>
    EventWriter<T>::EventWriter(EventPipe<T>& pipe)
        : mPipe(pipe)
    {
    }

    template <typename T>
    void EventWriter<T>::push(T event, unsigned int mask)
    {
        mPipe.push(event, mask);
    }

} // namespace cubos::core::ecs
