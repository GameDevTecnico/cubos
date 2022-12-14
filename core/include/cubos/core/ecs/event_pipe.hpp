#ifndef CUBOS_CORE_ECS_EVENT_PIPE_HPP
#define CUBOS_CORE_ECS_EVENT_PIPE_HPP

#include <vector>
#include <algorithm>
#include <atomic>
namespace cubos::core::ecs
{
    /// Event pipe implementation.
    /// @tparam T Event type.
    template <typename T> class EventPipe
    {
    public:
        /// Pushes event to event pipe, with its mask type.
        /// In case mask is not provided, the default mask will be 0.
        /// @param event Event which will be inserted into event pipe.
        /// @param mask Event mask.
        void push(T event, unsigned int mask = 0);

        /// Returns the event mask from event pipe at index.
        /// @param index Event index.
        unsigned int getEventMask(std::size_t index) const;

        /// Returns the event from event pipe at index, with read/write permissions.
        /// @param index Event index.
        T& getEvent(std::size_t index) const;

        /// Clears pipe event list.
        /// Only events that got read by all readers are deleted!
        void clear();

        /// Returns pipe event list size.
        std::size_t size() const;

        /// Adds a new reader to reader count.
        void addReader(); // FIXME: should be private

    private:
        /// Represents an Event, with its custom type, its mask and its read count.
        struct Event
        {
            T event;
            unsigned int mask;
            mutable std::atomic_size_t readCount{0};

            Event(T e, unsigned int m) : event(e), mask(m)
            {
            }
            Event(const Event& other)
            {
                *this = other;
            }
            Event& operator=(const Event& other)
            {
                this->event = other.event;
                this->mask = other.mask;
                this->readCount = other.readCount.load();
                return *this;
            }
        };

        /// List of events that are in the pipe.
        mutable std::vector<Event> events; // FIXME: std::deque for optimization

        /// Keeps track of how many readers the event pipe currently has.
        std::size_t readerCount;
    };

    // EventPipe implementation.

    template <typename T> void EventPipe<T>::push(T event, unsigned int mask)
    {
        this->events.push_back(Event(event, mask));
    }

    template <typename T> unsigned int EventPipe<T>::getEventMask(std::size_t index) const
    {
        return this->events.at(index).mask;
    }

    template <typename T> T& EventPipe<T>::getEvent(std::size_t index) const
    {
        Event& ev = this->events.at(index);
        ev.readCount++;
        return ev.event;
    }

    template <typename T> void EventPipe<T>::clear()
    {
        // FIXME: unefficient, we can use std::deque' pop_front which is O(1)
        auto shouldBeCleared = [readerCount = this->readerCount](const Event& event) {
            return event.readCount == readerCount;
        };
        this->events.erase(std::remove_if(this->events.begin(), this->events.end(), shouldBeCleared),
                           this->events.end());
    }

    template <typename T> std::size_t EventPipe<T>::size() const
    {
        return this->events.size();
    }

    template <typename T> void EventPipe<T>::addReader()
    {
        this->readerCount++;
    }

} // namespace cubos::core::ecs

#endif