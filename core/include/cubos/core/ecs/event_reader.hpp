#ifndef CUBOS_CORE_ECS_EVENT_READER_HPP
#define CUBOS_CORE_ECS_EVENT_READER_HPP

#include <cubos/core/ecs/event_pipe.hpp>

namespace cubos::core::ecs
{
    template <typename T, typename M> class EventReader
    {
    public:
        EventReader(const EventPipe<T>& pipe);

        // For iterators.

        auto begin()
        {
            return pipe.begin();
        }

        auto end()
        {
            return pipe.end();
        }

        auto begin() const
        {
            return pipe.begin();
        }

        auto end() const
        {
            return pipe.end();
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