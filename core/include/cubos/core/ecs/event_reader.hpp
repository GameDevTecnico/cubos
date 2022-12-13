#ifndef CUBOS_CORE_ECS_EVENT_READER_HPP
#define CUBOS_CORE_ECS_EVENT_READER_HPP

#include <cubos/core/ecs/event_pipe.hpp>

namespace cubos::core::ecs
{
    template <typename T, typename M> class EventReader
    {
    public:
        EventReader(const EventPipe<T>& pipe);

        auto begin()
        {
            return pipe.events.begin();
        }
        auto begin() const
        {
            return pipe.events.begin();
        }
        auto end()
        {
            return pipe.events.end();
        }
        auto end() const
        {
            return pipe.events.end();
        }

    private:
        const EventPipe<T>& pipe;
    };

    // Implementation.

    template <typename T, typename M> EventReader<T, M>::EventReader(const EventPipe<T>& pipe) : pipe(pipe)
    {
    }

} // namespace cubos::core::ecs

#endif