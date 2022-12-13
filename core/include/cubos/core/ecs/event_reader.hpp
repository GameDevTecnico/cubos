#ifndef CUBOS_CORE_ECS_EVENT_READER_HPP
#define CUBOS_CORE_ECS_EVENT_READER_HPP

#include <cubos/core/ecs/event_pipe.hpp>

namespace cubos::core::ecs
{
    /// Used to read events, filtering is also possible.
    /// @tparam T Event.
    /// @tparam M Event mask.
    template <typename T, std::size_t M> class EventReader
    {
    public:
        EventReader(const EventPipe<T>& pipe);

        /// EventReader custom iterator. Filters events depending on EventReader's M template parameter.
        class Iterator
        {
        public:
            Iterator(const EventPipe<T>& pipe, std::size_t mask, std::size_t index);
            const T& operator*() const;
            Iterator& operator++();
            bool operator==(const Iterator& other) const;
            bool operator!=(const Iterator& other) const;

        private:
            const EventPipe<T>& pipe;
            std::size_t mask;
            std::size_t index;
        };

        Iterator begin() const;
        Iterator end() const;

    private:
        const EventPipe<T>& pipe;
    };

    // EventReader implementation.

    template <typename T, std::size_t M> EventReader<T, M>::EventReader(const EventPipe<T>& pipe) : pipe(pipe)
    {
    }

    template <typename T, std::size_t M> EventReader<T, M>::Iterator EventReader<T, M>::begin() const
    {
        return Iterator(pipe, M, 0);
    }

    template <typename T, std::size_t M> EventReader<T, M>::Iterator EventReader<T, M>::end() const
    {
        return Iterator(pipe, M, pipe.size());
    }

    // EventReader::Iterator implementation.

    template <typename T, std::size_t M>
    EventReader<T, M>::Iterator::Iterator(const EventPipe<T>& pipe, std::size_t mask, std::size_t index)
        : pipe(pipe), mask(mask), index(index)
    {
        while (index < pipe.size() && pipe.getEventMask(index) != mask)
        {
            ++index;
        }
    }

    template <typename T, std::size_t M> const T& EventReader<T, M>::Iterator::operator*() const
    {
        return pipe.getEvent(index);
    }

    template <typename T, std::size_t M> EventReader<T, M>::Iterator& EventReader<T, M>::Iterator::operator++()
    {
        // Skip events that don't match the mask
        do
        {
            ++index;
        } while (index < pipe.size() && pipe.getEventMask(index) != mask);

        return *this;
    }

    template <typename T, std::size_t M> bool EventReader<T, M>::Iterator::operator==(const Iterator& other) const
    {
        // FIXME: known bug: this returns true for last element, which should not
        return mask == other.mask && index == other.index && &pipe == &other.pipe;
    }

    template <typename T, std::size_t M> bool EventReader<T, M>::Iterator::operator!=(const Iterator& other) const
    {
        return !(*this == other);
    }
} // namespace cubos::core::ecs

#endif