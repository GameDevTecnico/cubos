#include <cubos/engine/tools/entity_selector/plugin.hpp>

void cubos::engine::tools::entitySelectorPlugin(Cubos& cubos)
{
    cubos.addResource<cubos::engine::tools::EntitySelector>();
}
