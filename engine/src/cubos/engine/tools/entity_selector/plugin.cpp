#include <cubos/engine/tools/entity_selector/plugin.hpp>

void cubos::engine::plugins::tools::entitySelectorPlugin(Cubos& cubos)
{
    cubos.addResource<cubos::engine::plugins::tools::EntitySelector>();
}
