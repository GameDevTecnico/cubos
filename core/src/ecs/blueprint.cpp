#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/ecs/name.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/dictionary.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::ecs::Blueprint;
using cubos::core::ecs::Entity;
using cubos::core::ecs::EntityHash;
using cubos::core::ecs::Name;
using cubos::core::memory::AnyValue;
using cubos::core::memory::UnorderedBimap;
using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

Entity Blueprint::create(std::string name)
{
    CUBOS_ASSERT(!mBimap.containsRight(name), "An entity with the name '{}' already exists on the blueprint", name);
    CUBOS_ASSERT(validEntityName(name), "Blueprint entity name '{}' is invalid, read the docs", name);

    Entity entity{static_cast<uint32_t>(mBimap.size()), 0};
    mBimap.insert(entity, std::move(name));
    return entity;
}

void Blueprint::add(Entity entity, AnyValue component)
{
    CUBOS_ASSERT(component.type().get<ConstructibleTrait>().hasCopyConstruct(),
                 "Blueprint components must be copy constructible, but '{}' isn't", component.type().name());
    CUBOS_ASSERT(!component.type().has<EphemeralTrait>(), "Ephemeral components should not be stored in blueprints");

    // Sanity check to catch errors where the user passes an entity which doesn't belong to this blueprint.
    // We can't make sure it never happens, but we might as well catch some of the possible cases.
    CUBOS_ASSERT(mBimap.containsLeft(entity), "Entity wasn't created with this blueprint");

    if (!mComponents.contains(component.type()))
    {
        mComponents.insert(component.type(), {});
    }

    mComponents.at(component.type()).erase(entity);
    mComponents.at(component.type()).emplace(entity, std::move(component));
}

void Blueprint::relate(Entity fromEntity, Entity toEntity, AnyValue relation)
{
    CUBOS_ASSERT(relation.type().get<ConstructibleTrait>().hasCopyConstruct(),
                 "Blueprint relations must be copy constructible, but '{}' isn't", relation.type().name());
    CUBOS_ASSERT(!relation.type().has<EphemeralTrait>(), "Ephemeral relations should not be stored in blueprints");

    // Sanity check to catch errors where the user passes an entity which doesn't belong to this blueprint.
    // We can't make sure it never happens, but we might as well catch some of the possible cases.
    CUBOS_ASSERT(mBimap.containsLeft(fromEntity), "Entity wasn't created with this blueprint");
    CUBOS_ASSERT(mBimap.containsLeft(toEntity), "Entity wasn't created with this blueprint");

    if (relation.type().has<SymmetricTrait>() && fromEntity.index > toEntity.index)
    {
        // If the relation is symmetric, we always store the pair with the first index lower than the second.
        std::swap(fromEntity, toEntity);
    }

    if (!mRelations.contains(relation.type()))
    {
        mRelations.insert(relation.type(), {});
    }

    if (relation.type().has<TreeTrait>())
    {
        // If the relation is a tree relation, then we want to erase any previous outgoing relation from the entity.
        mRelations.at(relation.type()).erase(fromEntity);
    }

    mRelations.at(relation.type())[fromEntity].insert_or_assign(toEntity, std::move(relation));
}

void Blueprint::merge(const std::string& prefix, const Blueprint& other)
{
    other.instantiate(
        [&](std::string name) -> Entity {
            name = prefix + '.' + name;

            CUBOS_ASSERT(!mBimap.containsRight(name),
                         "A blueprint with the prefix '{}' was already merged to the blueprint", prefix);

            Entity entity{static_cast<uint32_t>(mBimap.size()), 0};
            mBimap.insert(entity, std::move(name));
            return entity;
        },
        [&](Entity entity, AnyValue component) { this->add(entity, std::move(component)); },
        [&](Entity fromEntity, Entity toEntity, AnyValue relation) {
            this->relate(fromEntity, toEntity, std::move(relation));
        },
        false);
}

void Blueprint::clear()
{
    mBimap.clear();
    mComponents.clear();
    mRelations.clear();
}

const UnorderedBimap<Entity, std::string, EntityHash>& Blueprint::bimap() const
{
    return mBimap;
}

static void convertToInstancedEntities(const std::unordered_map<Entity, Entity, EntityHash>& toInstanced,
                                       const Type& type, void* value)
{
    using namespace cubos::core::reflection;

    if (type.is<Entity>())
    {
        auto& entity = *static_cast<Entity*>(value);
        if (!entity.isNull())
        {
            CUBOS_ASSERT(toInstanced.contains(entity), "Entities stored in components/relations must either be null or "
                                                       "reference valid entities on their blueprints");
            entity = toInstanced.at(entity);
        }
    }
    else if (type.has<DictionaryTrait>())
    {
        const auto& trait = type.get<DictionaryTrait>();
        CUBOS_ASSERT(!trait.keyType().is<Entity>(),
                     "Dictionaries using entities as keys are not supported on blueprint components");

        for (auto [entryKey, entryValue] : trait.view(value))
        {
            convertToInstancedEntities(toInstanced, trait.valueType(), entryValue);
        }
    }
    else if (type.has<ArrayTrait>())
    {
        const auto& trait = type.get<ArrayTrait>();
        for (auto* element : trait.view(value))
        {
            convertToInstancedEntities(toInstanced, trait.elementType(), element);
        }
    }
    else if (type.has<FieldsTrait>())
    {
        const auto& trait = type.get<FieldsTrait>();
        for (auto [field, fieldValue] : trait.view(value))
        {
            convertToInstancedEntities(toInstanced, field->type(), fieldValue);
        }
    }
}

void Blueprint::instantiate(void* userData, Create create, Add add, Relate relate, bool withName) const
{
    // Instantiate our entities and create a map from them to their instanced counterparts.
    std::unordered_map<Entity, Entity, EntityHash> thisToInstance{};
    for (const auto& [entity, name] : mBimap)
    {
        thisToInstance.emplace(entity, create(userData, name));

        if (withName)
        {
            Name nameComponent{name};
            add(userData, thisToInstance.at(entity), AnyValue::moveConstruct(reflect<Name>(), (void*)&nameComponent));
        }
    }

    // Add copies of our components to their instantiated entities. Since the components themselves
    // may contain handle which point to entities in this blueprint, we must recurse into them and
    // convert them to point to the instantiated entities.
    // them and convert any handles
    for (const auto& [type, components] : mComponents)
    {
        for (const auto& [entity, component] : components)
        {
            auto copied = AnyValue::copyConstruct(component.type(), component.get());
            convertToInstancedEntities(thisToInstance, copied.type(), copied.get());
            add(userData, thisToInstance.at(entity), std::move(copied));
        }
    }

    // Do the same but for relations.
    for (const auto& [type, relations] : mRelations)
    {
        for (const auto& [fromEntity, outgoing] : relations)
        {
            for (const auto& [toEntity, relation] : outgoing)
            {
                auto copied = AnyValue::copyConstruct(relation.type(), relation.get());
                convertToInstancedEntities(thisToInstance, copied.type(), copied.get());
                relate(userData, thisToInstance.at(fromEntity), thisToInstance.at(toEntity), std::move(copied));
            }
        }
    }
}

bool Blueprint::validEntityName(const std::string& name)
{
    for (const auto& c : name)
    {
        bool isLowerAlpha = c >= 'a' && c <= 'z';
        bool isNumeric = c >= '0' && c <= '9';
        if (!isLowerAlpha && !isNumeric && c != '-')
        {
            return false;
        }
    }

    return true;
}
