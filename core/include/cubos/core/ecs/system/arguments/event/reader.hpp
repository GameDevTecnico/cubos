/// @file
/// @brief Class @ref cubos::core::ecs::EventReader.
/// @ingroup core-ecs-system-arguments

#pragma once

#include <cubos/core/ecs/system/arguments/event/pipe.hpp>
#include <cubos/core/ecs/system/fetcher.hpp>
#include <cubos/core/ecs/world.hpp>
#include <cubos/core/log.hpp>

namespace cubos::core::ecs
{
    /// @brief System arguments used to read events of type @p T.
    ///
    /// Filtering the received events by their mask is also possible via the parameter @p M.
    /// By default, the reader will read all events sent.
    ///
    /// @see Systems can send events using @ref EventWriter.
    /// @tparam T Event.
    /// @tparam M Filter mask.
    /// @ingroup core-ecs-system-arguments
    template <typename T, unsigned int M = DEFAULT_FILTER_MASK>
    class EventReader
    {
    public:
        /// @brief Constructs.
        ///
        /// Uses the given @p index to know which events it has already read. Increments it
        /// whenever it reads an event.
        ///
        /// @param pipe Event pipe to read events from.
        /// @param index Reference to the reader's index.
        EventReader(const EventPipe<T>& pipe, std::size_t& index);

        /// @brief Returns a reference to current event, and advances.
        /// @return Pointer to current event, or null if there are no more events.
        const T* read();

        /// @brief Used to iterate over events received by a reader.
        class Iterator
        {
        public:
            Iterator(EventReader<T, M>& reader, const T* ev, bool end);

            const T& operator*();
            Iterator& operator++();
            bool operator==(const Iterator& other) const;

        private:
            EventReader<T, M>& mReader;
            const T* mCurrentEvent;
            bool mEnd;
        };

        /// @brief Returns an iterator to the first event.
        /// @return Iterator.
        Iterator begin();

        /// @brief Returns an iterator to the end.
        /// @return Iterator.
        Iterator end();

    private:
        const EventPipe<T>& mPipe;
        std::size_t& mIndex;

        /// @brief Checks if given mask is valid to reader's one.
        /// @return True if mask is valid.
        bool matchesMask(decltype(M) mask) const;
    };

    // EventReader implementation.

    template <typename T, unsigned int M>
    EventReader<T, M>::EventReader(const EventPipe<T>& pipe, std::size_t& index)
        : mPipe(pipe)
        , mIndex(index)
    {
        static_assert(M != 0, "Invalid mask.");
    }

    template <typename T, unsigned int M>
    const T* EventReader<T, M>::read()
    {
        while (mIndex < mPipe.sentEvents())
        {
            std::pair<const T&, unsigned int> p = mPipe.get(mIndex++);
            if (matchesMask(p.second))
            {
                return &p.first;
            }
        }

        return nullptr;
    }

    template <typename T, unsigned int M>
    bool EventReader<T, M>::matchesMask(decltype(M) mask) const
    {
        return (M == ~0U) || (mask & M);
    }

    template <typename T, unsigned int M>
    typename EventReader<T, M>::Iterator EventReader<T, M>::begin()
    {
        // Return a new begin iterator only if we haven't read all events yet.
        if (mIndex >= mPipe.sentEvents())
        {
            return this->end();
        }
        return Iterator(*this, this->read(), false);
    }

    template <typename T, unsigned int M>
    typename EventReader<T, M>::Iterator EventReader<T, M>::end()
    {
        return Iterator(*this, nullptr, true);
    }

    // EventReader::Iterator implementation.

    template <typename T, unsigned int M>
    EventReader<T, M>::Iterator::Iterator(EventReader<T, M>& reader, const T* ev, bool end)
        : mReader(reader)
        , mCurrentEvent(ev)
        , mEnd(end)
    {
    }

    template <typename T, unsigned int M>
    const T& EventReader<T, M>::Iterator::operator*()
    {
        CUBOS_ASSERT(mCurrentEvent != nullptr, "Iterator out of bounds");
        return *mCurrentEvent;
    }

    template <typename T, unsigned int M>
    typename EventReader<T, M>::Iterator& EventReader<T, M>::Iterator::operator++()
    {
        mCurrentEvent = mReader.read();
        if (mCurrentEvent == nullptr)
        {
            mEnd = true;
        }
        return *this;
    }

    template <typename T, unsigned int M>
    bool EventReader<T, M>::Iterator::operator==(const Iterator& other) const
    {
        return mEnd == other.mEnd;
    }

    template <typename T>
    class SystemFetcher<EventReader<T>>
    {
    public:
        static inline constexpr bool ConsumesOptions = false;

        SystemFetcher(World& world, const SystemOptions& /*options*/)
            : mPipe{world.read<EventPipe<T>>().get()}
        {
        }

        void analyze(SystemAccess& /*access*/) const
        {
            // FIXME: when we add resources to the world type registry, we should the event type id to the access
            // object.
        }

        EventReader<T> fetch(CommandBuffer& /*cmdBuffer*/)
        {
            return {mPipe, mIndex};
        }

    private:
        const EventPipe<T>& mPipe;
        std::size_t mIndex{0};
    };
} // namespace cubos::core::ecs
