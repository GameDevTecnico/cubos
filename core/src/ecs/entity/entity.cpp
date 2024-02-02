#include <cubos/core/data/old/deserializer.hpp>
#include <cubos/core/data/old/serialization_map.hpp>
#include <cubos/core/data/old/serializer.hpp>
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

template <>
void cubos::core::data::old::serialize<Entity>(Serializer& ser, const Entity& obj, const char* name)
{
    if (ser.context().has<SerializationMap<Entity, std::string, EntityHash>>())
    {
        if (obj.isNull())
        {
            ser.write("null", name);
            return;
        }

        auto& map = ser.context().get<SerializationMap<Entity, std::string, EntityHash>>();
        ser.write(map.getId(obj), name);
    }
    else
    {
        ser.beginObject(name);
        ser.write(obj.index, "index");
        ser.write(obj.generation, "generation");
        ser.endObject();
    }
}

template <>
void cubos::core::data::old::deserialize<Entity>(Deserializer& des, Entity& obj)
{
    if (des.context().has<SerializationMap<Entity, std::string, EntityHash>>())
    {
        std::string name;
        des.read(name);
        if (name == "null")
        {
            obj = Entity();
            return;
        }

        auto& map = des.context().get<SerializationMap<Entity, std::string, EntityHash>>();
        if (map.hasId(name))
        {
            obj = map.getRef(name);
        }
        else
        {
            CUBOS_WARN("No such entity '{}'", name);
            des.fail();
        }
    }
    else
    {
        des.beginObject();
        des.read(obj.index);
        des.read(obj.generation);
        des.endObject();
    }
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
