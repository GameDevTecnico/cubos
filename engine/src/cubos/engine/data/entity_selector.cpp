#include <cubos/engine/data/entity_selector.hpp>

using namespace cubos::engine::data;
using namespace cubos::core::ecs;

void EntitySelector::Select(Entity& entity)
{
    this->selection = std::make_shared<Entity>(entity);
}

void EntitySelector::Unselect()
{
    this->selection = nullptr;
}

std::shared_ptr<cubos::core::ecs::Entity> EntitySelector::GetSelection()
{
    return this->selection;
}