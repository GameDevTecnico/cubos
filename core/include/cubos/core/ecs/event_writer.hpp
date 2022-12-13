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

    private:
        EventPipe<T>& pipe;
    };

    // Implementation.

    template <typename T> EventWriter<T>::EventWriter(EventPipe<T>& pipe) : pipe(pipe)
    {
    }

    template <typename T> void EventWriter<T>::push(T event)
    {
        this->pipe.push(event);
    }

} // namespace cubos::core::ecs

#endif