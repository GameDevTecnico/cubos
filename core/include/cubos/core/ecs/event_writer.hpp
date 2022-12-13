#ifndef CUBOS_CORE_ECS_EVENT_WRITER_HPP
#define CUBOS_CORE_ECS_EVENT_WRITER_HPP

#include <cubos/core/ecs/event_pipe.hpp>

namespace cubos::core::ecs
{
    template <typename T> class EventWriter
    {
    public:
        EventWriter(EventPipe<T>& pipe);

        /// Pushes event to event pipe.
        /// @param event Event which will be inserted into event pipe.
        void push(T event);

        /// Pushes event to event pipe, with its mask type.
        /// @param event Event which will be inserted into event pipe.
        /// @param mask Event mask.
        void push(T event, std::size_t mask);

    private:
        EventPipe<T>& pipe;
    };

    // EventWriter implementation.

    template <typename T> EventWriter<T>::EventWriter(EventPipe<T>& pipe) : pipe(pipe)
    {
    }

    template <typename T> void EventWriter<T>::push(T event)
    {
        this->pipe.push(event);
    }

    template <typename T> void EventWriter<T>::push(T event, std::size_t mask)
    {
        this->pipe.push(event, mask);
    }

} // namespace cubos::core::ecs

#endif