#ifndef CUBOS_CORE_ECS_EVENT_READER_HPP
#define CUBOS_CORE_ECS_EVENT_READER_HPP

#include <cubos/core/ecs/event_pipe.hpp>
#include <optional>

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
        /// It returns nullopt if there are no more events to read!
        std::optional<std::reference_wrapper<T>> read();

        /// EventReader custom iterator.
        class Iterator
        {
        public:
            Iterator(EventReader<T, M>& reader, std::optional<std::reference_wrapper<T>> ev, bool end);

            T& operator*();
            Iterator& operator++();
            bool operator==(const Iterator& other);
            bool operator!=(const Iterator& other);

        private:
            EventReader<T, M>& reader;
            std::optional<std::reference_wrapper<T>> currentEvent;
            bool end;
        };

        Iterator begin();
        Iterator end();

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

    template <typename T, unsigned int M> EventReader<T, M>::Iterator EventReader<T, M>::begin()
    {
        // return a new begin iterator only if we did not read all events yet(?)
        return (this->index >= this->pipe.size()) ? this->end() : Iterator(*this, this->read(), false);
    }

    template <typename T, unsigned int M> EventReader<T, M>::Iterator EventReader<T, M>::end()
    {
        return Iterator(*this, std::nullopt, true);
    }

    // EventReader::Iterator implementation.

    template <typename T, unsigned int M>
    EventReader<T, M>::Iterator::Iterator(EventReader<T, M>& r, std::optional<std::reference_wrapper<T>> ev, bool e)
        : reader(r), currentEvent(ev), end(e)
    {
    }

    template <typename T, unsigned int M> T& EventReader<T, M>::Iterator::operator*()
    {
        return currentEvent->get();
    }

    template <typename T, unsigned int M> EventReader<T, M>::Iterator& EventReader<T, M>::Iterator::operator++()
    {
        currentEvent = reader.read();
        if (currentEvent == std::nullopt)
        {
            this->end = true;
        }
        return *this;
    }

    template <typename T, unsigned int M> bool EventReader<T, M>::Iterator::operator==(const Iterator& other)
    {
        return this->end == other.end; // FIXME
    }

    template <typename T, unsigned int M> bool EventReader<T, M>::Iterator::operator!=(const Iterator& other)
    {
        return !(*this == other);
    }

} // namespace cubos::core::ecs

#endif
