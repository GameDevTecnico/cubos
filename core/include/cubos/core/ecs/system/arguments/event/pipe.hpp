/// @file
/// @brief Resource @ref cubos::core::ecs::EventPipe.
/// @ingroup core-ecs-system-arguments

#pragma once

#define DEFAULT_FILTER_MASK ~0u
#define DEFAULT_PUSH_MASK 0

#include <atomic>
#include <deque>

#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

namespace cubos::core::ecs
{
    /// @brief Resource which stores events of type @p T.
    /// @note This resource is meant to be used through @ref EventReader and @ref EventWriter.
    /// @tparam T Event type.
    /// @ingroup core-ecs-system-arguments
    template <typename T>
    class EventPipe
    {
    public:
        CUBOS_REFLECT;

        /// @brief Pushes an event into the event pipe.
        /// @param event Event.
        /// @param mask Mask.
        void push(T event, unsigned int mask = DEFAULT_PUSH_MASK);

        /// @brief Returns the event mask from event pipe at the given @p index.
        /// @param index Event index.
        /// @return Event mask.
        unsigned int getEventMask(std::size_t index) const;

        /// @brief Returns the event and mask with the given @p index.
        /// @param index Event index.
        /// @return Event and mask.
        std::pair<const T&, unsigned int> get(std::size_t index) const;

        /// @brief Clears events that have been read by all readers.
        void clear();

        /// @brief Returns the number of events that already were sent.
        /// @return Number of events that already were sent.
        std::size_t sentEvents() const;

        /// @brief Returns the number of events that are present on the pipe.
        /// @return Number of events that are present on the pipe.
        std::size_t size() const;

        /// @brief Adds a new reader to reader count.
        ///
        /// This is necessary to keep track of when its okay to delete events on @ref clear().
        ///
        /// @note This is called on @ref impl::SystemFetcher::prepare().
        /// @see EventReader
        void addReader();

        /// @brief Removes a reader from reader count.
        /// @see EventReader
        void removeReader();

    private:
        /// @brief Stores an event, its mask and its read count.
        struct Event
        {
            T event;
            unsigned int mask;
            mutable std::atomic_size_t readCount{0};

            Event(T e, unsigned int m)
                : event(std::move(e))
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

        /// @brief List of events that are in the pipe.
        std::deque<Event> mEvents;

        /// @brief How many readers the event pipe currently has.
        std::size_t mReaderCount;

        /// @brief How many events were deleted.
        std::size_t mDeletedEvents{0};
    };

    // EventPipe implementation.

    CUBOS_REFLECT_TEMPLATE_IMPL((typename T), (EventPipe<T>))
    {
        return reflection::Type::create("cubos::core::ecs::EventPipe<" + reflection::reflect<T>().name() + ">")
            .with(reflection::ConstructibleTrait::typed<EventPipe<T>>().withMoveConstructor().build());
    }

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
