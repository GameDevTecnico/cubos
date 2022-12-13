#ifndef CUBOS_CORE_ECS_EVENT_PIPE_HPP
#define CUBOS_CORE_ECS_EVENT_PIPE_HPP

#include <vector>

namespace cubos::core::ecs
{
    /// Event pipe implementation.
    /// @tparam T Event type.
    template <typename T> class EventPipe
    {
    public:
        /// Pushes event to event pipe.
        /// @param event Event which will be inserted into event pipe.
        void push(T event);

        /// Clears pipe event list.
        void clear();

        /// FIXME: this should be private
        /// its public for now until iterators are not implemented into event_reader
        std::vector<T> events;

    private:
    };

    // Implementation.

    template <typename T> void EventPipe<T>::push(T event)
    {
        this->events.push_back(event);
    }

    template <typename T> void EventPipe<T>::clear()
    {
        this->events.clear();
    }

} // namespace cubos::core::ecs

#endif