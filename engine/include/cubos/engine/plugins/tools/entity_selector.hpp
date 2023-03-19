#ifndef CUBOS_ENGINE_PLUGINS_ENTITY_SELECTOR_HPP
#define CUBOS_ENGINE_PLUGINS_ENTITY_SELECTOR_HPP

#include <cubos/engine/cubos.hpp>

#include <cubos/core/ecs/entity_manager.hpp>

namespace cubos::engine::plugins::tools
{
    /// Resource used to identify the currently selected entity. Used by multiple editor plugins.
    struct EntitySelector
    {
        cubos::core::ecs::Entity selection;
    };

    /// Plugin that allows having a singular entity be selected for inspection
    /// @param cubos CUBOS. main class
    void entitySelectorPlugin(Cubos& cubos);
}; // namespace cubos::engine::plugins::tools

#endif // CUBOS_ENGINE_PLUGINS_ENTITY_SELECTOR_HPP
