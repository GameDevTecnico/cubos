#ifndef CUBOS_CORE_ECS_EVENT_READER_HPP
#define CUBOS_CORE_ECS_EVENT_READER_HPP

#include <cubos/core/ecs/event_pipe.hpp>
#include <optional>

namespace cubos::core::ecs
{
    /// Used to read events, filtering is also possible via M parameter.
    /// In case M template parameter is not provided, it will read all events (no filtering).
    /// @tparam T Event.
    /// @tparam M Event mask.
    template <typename T, unsigned int M = DEFAULT_FILTER_MASK> class EventReader
    {
    public:
        EventReader(const EventPipe<T>& pipe, std::size_t& index);

        /// Returns a reference to current event, and advances.
        /// It returns nullopt if there are no more events to read!
        std::optional<std::reference_wrapper<const T>> read();

        /// EventReader custom iterator.
        class Iterator
        {
        public:
            Iterator(EventReader<T, M>& reader, std::optional<std::reference_wrapper<const T>> ev, bool end);

            const T& operator*();
            Iterator& operator++();
            bool operator==(const Iterator& other);
            bool operator!=(const Iterator& other);

        private:
            EventReader<T, M>& reader;
            std::optional<std::reference_wrapper<const T>> currentEvent;
            bool end;
        };

        Iterator begin();
        Iterator end();

    private:
        const EventPipe<T>& pipe;
        std::size_t& index{};

        /// Checks if given mask is valid to reader's one.
        /// @return True if mask is valid.
        bool matchesMask(decltype(M) mask) const;
    };

    // EventReader implementation.

    template <typename T, unsigned int M>
    EventReader<T, M>::EventReader(const EventPipe<T>& pipe, std::size_t& index) : pipe(pipe), index(index)
    {
        static_assert(M != 0, "Invalid mask.");
    }

    template <typename T, unsigned int M> std::optional<std::reference_wrapper<const T>> EventReader<T, M>::read()
    {
        while (this->index < this->pipe.sentEvents())
        {
            std::pair<const T&, unsigned int> p = pipe.get(this->index++);
            auto& event = p.first;
            auto mask = p.second;
            if (matchesMask(mask))
            {
                // return std::optional<std::reference_wrapper<const T>>(std::reference_wrapper<const T>(event));
                return std::make_optional(std::reference_wrapper<const T>(event));
            }
        }

        return std::nullopt;
    }

    template <typename T, unsigned int M> bool EventReader<T, M>::matchesMask(decltype(M) eventMask) const
    {
        return M == ~0 ? true : eventMask & M;
    }

    template <typename T, unsigned int M> typename EventReader<T, M>::Iterator EventReader<T, M>::begin()
    {
        // return a new begin iterator only if we did not read all events yet(?)
        return (this->index >= this->pipe.sentEvents()) ? this->end() : Iterator(*this, this->read(), false);
    }

    template <typename T, unsigned int M> typename EventReader<T, M>::Iterator EventReader<T, M>::end()
    {
        return Iterator(*this, std::nullopt, true);
    }

    // EventReader::Iterator implementation.

    template <typename T, unsigned int M>
    EventReader<T, M>::Iterator::Iterator(EventReader<T, M>& r, std::optional<std::reference_wrapper<const T>> ev,
                                          bool e)
        : reader(r), currentEvent(ev), end(e)
    {
    }

    template <typename T, unsigned int M> const T& EventReader<T, M>::Iterator::operator*()
    {
        return currentEvent->get();
    }

    template <typename T, unsigned int M>
    typename EventReader<T, M>::Iterator& EventReader<T, M>::Iterator::operator++()
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
        return this->end == other.end;
    }

    template <typename T, unsigned int M> bool EventReader<T, M>::Iterator::operator!=(const Iterator& other)
    {
        return !(*this == other);
    }

} // namespace cubos::core::ecs

#endif
