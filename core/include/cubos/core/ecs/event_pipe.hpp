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

        /// Pushes event to event pipe, with its mask type.
        /// @param event Event which will be inserted into event pipe.
        /// @param mask Event mask.
        void push(T event, unsigned int mask);

        /// Returns the event mask from event pipe at index.
        /// @param index Event index.
        unsigned int getEventMask(std::size_t index) const;

        /// Returns the event from event pipe at index.
        /// @param index Event index.
        const T& getEvent(std::size_t index) const;

        /// Clears pipe event list.
        void clear();

        /// Returns pipe event list size.
        std::size_t size() const;

    private:
        /// Represents an Event, with its custom type and its mask.
        struct Event
        {
            T event;
            unsigned int mask;
        };

        /// List of events that are in the pipe.
        std::vector<Event> events;
    };

    // EventPipe implementation.

    template <typename T> void EventPipe<T>::push(T event)
    {
        this->events.push_back({.event = event, .mask = 0});
    }

    template <typename T> void EventPipe<T>::push(T event, unsigned int mask)
    {
        this->events.push_back({.event = event, .mask = mask});
    }

    template <typename T> unsigned int EventPipe<T>::getEventMask(std::size_t index) const
    {
        return this->events.at(index).mask;
    }

    template <typename T> const T& EventPipe<T>::getEvent(std::size_t index) const
    {
        return this->events.at(index).event;
    }

    template <typename T> void EventPipe<T>::clear()
    {
        this->events.clear();
    }

    template <typename T> std::size_t EventPipe<T>::size() const
    {
        return this->events.size();
    }

} // namespace cubos::core::ecs

#endif