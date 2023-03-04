#ifndef CUBOS_ENGINE_DATA_ENTITY_SELECTOR_HPP
#define CUBOS_ENGINE_DATA_ENTITY_SELECTOR_HPP

#include <cubos/core/ecs/entity_manager.hpp>

namespace cubos::engine::data
{
    class EntitySelector
    {
    public:
        cubos::core::ecs::Entity selection;
    };
} // namespace cubos::engine::data

#endif // CUBOS_ENGINE_DATA_ENTITY_SELECTOR_HPP