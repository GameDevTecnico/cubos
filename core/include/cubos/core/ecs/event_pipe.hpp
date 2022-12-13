#ifndef CUBOS_CORE_ECS_EVENT_PIPE_HPP
#define CUBOS_CORE_ECS_EVENT_PIPE_HPP

#include <vector>
#include <algorithm>
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

        /// Returns the event from event pipe at index.
        /// @param index Event index.
        const T& getEvent(std::size_t index) const;

        /// Clears pipe event list.
        void clear();

        /// Returns pipe event list size.
        std::size_t size() const;

        /// Adds a new reader to reader count.
        void addReader(); // TODO: should be private

    private:
        /// Represents an Event, with its custom type, its mask and its read count.
        struct Event
        {
            T event;
            unsigned int mask;
            std::size_t readCount;
        };

        /// Deletes an event if it already got read by all readers.
        const T deleteIfReadByAll(Event& event);

        /// List of events that are in the pipe.
        std::vector<Event> events;

        /// Keeps track of how many readers the event pipe currently has.
        std::size_t readerCount;
    };

    // EventPipe implementation.

    template <typename T> void EventPipe<T>::push(T event, unsigned int mask)
    {
        this->events.push_back({.event = event, .mask = mask, .readCount = 0});
    }

    template <typename T> unsigned int EventPipe<T>::getEventMask(std::size_t index) const
    {
        return this->events.at(index).mask;
    }

    template <typename T> const T& EventPipe<T>::getEvent(std::size_t index) const
    {
        // auto result = deleteIfReadByAll(this->events.at(index));
        // return result;
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

    template <typename T> void EventPipe<T>::addReader()
    {
        this->readerCount++;
    }

    template <typename T> const T EventPipe<T>::deleteIfReadByAll(EventPipe<T>::Event& event)
    {
        if (++event.readCount == this->readerCount)
        {
            Event copy = event;
            auto it = std::find(this->events.begin(), this->events.end(), event);
            this->events.erase(it);
            return copy.event;
        }
        else
        {
            return event.event;
        }
    }

} // namespace cubos::core::ecs

#endif