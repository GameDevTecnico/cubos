#ifndef CUBOS_CORE_ECS_SYSTEM_HPP
#define CUBOS_CORE_ECS_SYSTEM_HPP

#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    class System
    {
    public:
        virtual void init(World& world);
        virtual void update(World& world);
    };
} // namespace cubos::core::ecs
#endif // CUBOS_CORE_ECS_SYSTEM_HPP
