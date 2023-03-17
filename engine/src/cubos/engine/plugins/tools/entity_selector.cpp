#include <cubos/engine/plugins/tools/entity_selector.hpp>
#include <cubos/engine/data/entity_selector.hpp>

void cubos::engine::plugins::tools::entitySelectorPlugin(Cubos& cubos)
{
    cubos.addResource<cubos::engine::data::EntitySelector>();
}