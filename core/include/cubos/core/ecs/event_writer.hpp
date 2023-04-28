#ifndef CUBOS_CORE_ECS_EVENT_WRITER_HPP
#define CUBOS_CORE_ECS_EVENT_WRITER_HPP

#include <cubos/core/ecs/event_pipe.hpp>

namespace cubos::core::ecs
{
    template <typename T>
    class EventWriter
    {
    public:
        EventWriter(EventPipe<T>& pipe);

        /// Pushes event to event pipe, with its mask type.
        /// In case mask is not provided, the default mask will be 0. (no filtering possible)
        /// @param event Event which will be inserted into event pipe.
        /// @param mask Event mask.
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

#endif
