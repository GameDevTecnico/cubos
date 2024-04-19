#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::ecs::Entity;
using cubos::core::ecs::EntityHash;
using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::Type;

CUBOS_REFLECT_IMPL(Entity)
{
    return Type::create("cubos::core::ecs::Entity")
        .with(ConstructibleTrait::typed<Entity>().withBasicConstructors().build())
        .with(FieldsTrait().withField("index", &Entity::index).withField("generation", &Entity::generation));
}

Entity::Entity()
{
    this->index = UINT32_MAX;
    this->generation = UINT32_MAX;
}

Entity::Entity(uint32_t index, uint32_t generation)
    : index(index)
    , generation(generation)
{
    // Do nothing.
}

bool Entity::isNull() const
{
    return this->index == UINT32_MAX && this->generation == UINT32_MAX;
}
