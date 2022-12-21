#ifndef CUBOS_CORE_ECS_EVENT_READER_HPP
#define CUBOS_CORE_ECS_EVENT_READER_HPP

#include <cubos/core/ecs/event_pipe.hpp>

namespace cubos::core::ecs
{
    /// Used to read events, filtering is also possible via M param.
    /// @tparam T Event.
    /// @tparam M Event mask.
    template <typename T, unsigned int M> class EventReader
    {
    public:
        EventReader(EventPipe<T>& pipe);

        /// Returns a reference to current event, and advances.
        /// It will return nullopt if there are no more events to read!
        std::optional<std::reference_wrapper<T>> read();

        /// EventReader custom iterator. Filters events depending on EventReader's M template parameter.
        class Iterator
        {
        public:
            Iterator(const EventPipe<T>& pipe, decltype(M) mask, std::size_t index);

            T& operator*() const;
            Iterator& operator++();
            bool operator==(const Iterator& other) const;
            bool operator!=(const Iterator& other) const;

        private:
            const EventPipe<T>& pipe;
            decltype(M) mask;
            std::size_t index;
        };

        Iterator begin() const;
        Iterator end() const;

    private:
        EventPipe<T>& pipe;
        std::size_t index{};

        /// Checks if given mask is valid to reader's one.
        /// @return True if mask is valid.
        bool matchesMask(decltype(M) mask) const;
    };

    // EventReader implementation.

    template <typename T, unsigned int M> EventReader<T, M>::EventReader(EventPipe<T>& pipe) : pipe(pipe)
    {
        this->pipe.addReader();
    }

    template <typename T, unsigned int M> std::optional<std::reference_wrapper<T>> EventReader<T, M>::read()
    {
        while (this->index < this->pipe.size())
        {
            auto [event, mask] = pipe.get(this->index++);
            if (matchesMask(mask))
            {
                return event;
            }
        }

        return std::nullopt;
    }

    template <typename T, unsigned int M> EventReader<T, M>::Iterator EventReader<T, M>::begin() const
    {
        return Iterator(this->pipe, M, 0);
    }

    template <typename T, unsigned int M> EventReader<T, M>::Iterator EventReader<T, M>::end() const
    {
        return Iterator(this->pipe, M, this->pipe.size());
    }

    template <typename T, unsigned int M> bool EventReader<T, M>::matchesMask(decltype(M) eventMask) const
    {
        if (eventMask == 0 && M == 0)
        {
            return true;
        }

        if (!(eventMask & M) || (eventMask & M) != eventMask)
        {
            return false;
        }

        return true;
    }

    // EventReader::Iterator implementation.

    template <typename T, unsigned int M>
    EventReader<T, M>::Iterator::Iterator(const EventPipe<T>& pipe, decltype(M) mask, std::size_t index)
        : pipe(pipe), mask(mask), index(index)
    {
        while (this->index < this->pipe.size())
        {
            this->index++;
        }
    }

    template <typename T, unsigned int M> T& EventReader<T, M>::Iterator::operator*() const
    {
        return this->pipe.get(this->index).first;
    }

    template <typename T, unsigned int M> EventReader<T, M>::Iterator& EventReader<T, M>::Iterator::operator++()
    {
        do
        {
            this->index++;
        } while (this->index < this->pipe.size());

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

} // namespace cubos::core::ecs

#endif