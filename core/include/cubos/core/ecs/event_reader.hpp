#ifndef CUBOS_CORE_ECS_EVENT_READER_HPP
#define CUBOS_CORE_ECS_EVENT_READER_HPP

#include <cubos/core/ecs/event_pipe.hpp>

namespace cubos::core::ecs
{
    /// Used to read events, filtering is also possible.
    /// @tparam T Event.
    /// @tparam M Event mask.
    template <typename T, unsigned int M> class EventReader
    {
    public:
        EventReader(const EventPipe<T>& pipe);

        /// EventReader custom iterator. Filters events depending on EventReader's M template parameter.
        class Iterator
        {
        public:
            Iterator(const EventPipe<T>& pipe, decltype(M) mask, std::size_t index);

            const T& operator*() const;
            Iterator& operator++();
            bool operator==(const Iterator& other) const;
            bool operator!=(const Iterator& other) const;

        private:
            /// Checks if current event mask is valid.
            /// @return True if current event iterated matches the given mask.
            bool matchesMask() const;

            const EventPipe<T>& pipe;
            decltype(M) mask;
            std::size_t index;
        };

        Iterator begin() const;
        Iterator end() const;

    private:
        const EventPipe<T>& pipe;
    };

    // EventReader implementation.

    template <typename T, unsigned int M> EventReader<T, M>::EventReader(const EventPipe<T>& pipe) : pipe(pipe)
    {
    }

    template <typename T, unsigned int M> EventReader<T, M>::Iterator EventReader<T, M>::begin() const
    {
        return Iterator(this->pipe, M, 0);
    }

    template <typename T, unsigned int M> EventReader<T, M>::Iterator EventReader<T, M>::end() const
    {
        return Iterator(this->pipe, M, this->pipe.size());
    }

    // EventReader::Iterator implementation.

    template <typename T, unsigned int M>
    EventReader<T, M>::Iterator::Iterator(const EventPipe<T>& pipe, decltype(M) mask, std::size_t index)
        : pipe(pipe), mask(mask), index(index)
    {
        while (this->index < this->pipe.size() && !matchesMask())
        {
            this->index++;
        }
    }

    template <typename T, unsigned int M> const T& EventReader<T, M>::Iterator::operator*() const
    {
        return this->pipe.getEvent(this->index);
    }

    template <typename T, unsigned int M> EventReader<T, M>::Iterator& EventReader<T, M>::Iterator::operator++()
    {
        // Skip events that don't match the mask
        do
        {
            this->index++;
        } while (this->index < this->pipe.size() && !matchesMask());

        return *this;
    }

    template <typename T, unsigned int M> bool EventReader<T, M>::Iterator::operator==(const Iterator& other) const
    {
        return this->mask == other.mask && this->index == other.index && &(this->pipe) == &other.pipe;
    }

    template <typename T, unsigned int M> bool EventReader<T, M>::Iterator::operator!=(const Iterator& other) const
    {
        return !(*this == other);
    }

    template <typename T, unsigned int M> bool EventReader<T, M>::Iterator::matchesMask() const
    {
        auto eventMask = this->pipe.getEventMask(this->index);

        if (eventMask == 0 && this->mask == 0)
        {
            return true;
        }

        if (!(eventMask & this->mask))
        {
            return false;
        }

        if ((eventMask & this->mask) != eventMask)
        {
            return false;
        }

        return true;
    }
} // namespace cubos::core::ecs

#endif