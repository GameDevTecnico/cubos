/// @file
/// @brief Class @ref cubos::core::ecs::EventWriter.
/// @ingroup core-ecs-system-arguments

#pragma once

#include <cubos/core/ecs/system/arguments/event/pipe.hpp>
#include <cubos/core/ecs/system/fetcher.hpp>
#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    /// @brief System argument which allows the system to send events of type @p T to other
    /// systems.
    /// @see Systems can read sent events using @ref EventReader.
    /// @tparam T
    /// @ingroup core-ecs-system-arguments
    template <typename T>
    class EventWriter
    {
    public:
        /// @brief Constructs.
        /// @param pipe Event pipe to write events to.
        EventWriter(EventPipe<T>& pipe)
            : mPipe(pipe)
        {
        }

        /// @brief Sends the given @p event to the event pipe with the given @p mask.
        /// @param event Event.
        /// @param mask Mask.
        void push(T event, unsigned int mask = DEFAULT_PUSH_MASK)
        {
            mPipe.push(event, mask);
        }

    private:
        EventPipe<T>& mPipe;
    };

    template <typename T>
    class SystemFetcher<EventWriter<T>>
    {
    public:
        static inline constexpr bool ConsumesOptions = false;

        SystemFetcher(World& world, const SystemOptions& /*options*/)
            : mPipe{world.resource<EventPipe<T>>()}
        {
        }

        void analyze(SystemAccess& /*access*/) const
        {
            // FIXME: when we add resources to the world type registry, we should the event type id to the access
            // object.
        }

        EventWriter<T> fetch(const SystemContext& /*ctx*/)
        {
            return {mPipe};
        }

    private:
        EventPipe<T>& mPipe;
    };
} // namespace cubos::core::ecs
