#ifndef CUBOS_ENGINE_PLUGINS_ENTITY_SELECTOR_HPP
#define CUBOS_ENGINE_PLUGINS_ENTITY_SELECTOR_HPP

#include <cubos/engine/cubos.hpp>

using namespace cubos::engine;

namespace cubos::engine::plugins::tools
{
    /// Plugin that allows having a singular entity be selected for inspection
    /// @param cubos CUBOS. main class
    void entitySelectorPlugin(Cubos& cubos);
}; // namespace cubos::engine::plugins::tools

#endif // CUBOS_ENGINE_PLUGINS_ENTITY_SELECTOR_HPP