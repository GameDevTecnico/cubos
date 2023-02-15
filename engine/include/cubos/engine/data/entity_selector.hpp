#ifndef CUBOS_ENGINE_DATA_ENTITY_SELECTOR_HPP
#define CUBOS_ENGINE_DATA_ENTITY_SELECTOR_HPP

#include <cubos/core/ecs/entity_manager.hpp>

namespace cubos::engine::data
{
    class EntitySelector
    {
    private:
        std::shared_ptr<cubos::core::ecs::Entity> selection;

    public:
        void Select(core::ecs::Entity& entity);
        void Unselect();
        std::shared_ptr<cubos::core::ecs::Entity> GetSelection();
    };
} // namespace cubos::engine::data

#endif // CUBOS_ENGINE_DATA_ENTITY_SELECTOR_HPP