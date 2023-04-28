#ifndef CUBOS_CORE_ECS_EVENT_PIPE_HPP
#define CUBOS_CORE_ECS_EVENT_PIPE_HPP

#define DEFAULT_FILTER_MASK ~0u
#define DEFAULT_PUSH_MASK 0

#include <atomic>
#include <deque>

namespace cubos::core::ecs
{
    /// Event pipe implementation.
    /// @tparam T Event type.
    template <typename T>
    class EventPipe
    {
    public:
        /// Pushes event to event pipe, with its mask type.
        /// In case mask is not provided, the default mask will be 0.
        /// @param event Event which will be inserted into event pipe.
        /// @param mask Event mask.
        void push(T event, unsigned int mask = DEFAULT_PUSH_MASK);

        /// Returns the event mask from event pipe at index.
        /// @param index Event index.
        unsigned int getEventMask(std::size_t index) const;

        /// Returns the event from event pipe at index (with read/write permissions) and its mask.
        /// @param index Event index.
        std::pair<const T&, unsigned int> get(std::size_t index) const;

        /// Clears pipe event list.
        /// Only events that got read by all readers are deleted!
        void clear();

        /// Returns the number of events that already were sent.
        std::size_t sentEvents() const;

        /// Returns the number of events that are present on the pipe.
        std::size_t size() const;

        /// Adds a new reader to reader count.
        void addReader();

        /// Removes a reader from reader count.
        void removeReader();

    private:
        /// Represents an Event, with its custom type, its mask and its read count.
        struct Event
        {
            T event;
            unsigned int mask;
            mutable std::atomic_size_t readCount{0};

            Event(T e, unsigned int m)
                : event(e)
                , mask(m)
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
        std::deque<Event> mEvents;

        /// Keeps track of how many readers the event pipe currently has.
        std::size_t mReaderCount;

        /// Keeps track of how many events were deleted.
        std::size_t mDeletedEvents{0};
    };

    // EventPipe implementation.

    template <typename T>
    void EventPipe<T>::push(T event, unsigned int mask)
    {
        mEvents.push_back(Event(event, mask));
    }

    template <typename T>
    unsigned int EventPipe<T>::getEventMask(std::size_t index) const
    {
        index = index - mDeletedEvents;
        return mEvents.at(index).mask;
    }

    template <typename T>
    std::pair<const T&, unsigned int> EventPipe<T>::get(std::size_t index) const
    {
        index = index - mDeletedEvents;
        const Event& ev = mEvents.at(index);
        ev.readCount++;
        return std::pair<const T&, unsigned int>(ev.event, ev.mask);
    }

    template <typename T>
    void EventPipe<T>::clear()
    {
        while (!mEvents.empty() && mEvents.front().readCount == mReaderCount)
        {
            mEvents.pop_front();
            mDeletedEvents++;
        }
    }

    template <typename T>
    std::size_t EventPipe<T>::sentEvents() const
    {
        return mEvents.size() + mDeletedEvents;
    }

    template <typename T>
    std::size_t EventPipe<T>::size() const
    {
        return mEvents.size();
    }

    template <typename T>
    void EventPipe<T>::addReader()
    {
        mReaderCount++;
    }

    template <typename T>
    void EventPipe<T>::removeReader()
    {
        if (mReaderCount > 0)
        {
            mReaderCount--;
        }
    }

} // namespace cubos::core::ecs

#endif
