#include <tesseratos/entity_selector/plugin.hpp>

using cubos::engine::Cubos;

void tesseratos::entitySelectorPlugin(Cubos& cubos)
{
    cubos.addResource<EntitySelector>();
}
