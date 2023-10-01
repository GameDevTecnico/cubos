#include <cubos/core/data/old/deserializer.hpp>
#include <cubos/core/data/old/serialization_map.hpp>
#include <cubos/core/data/old/serializer.hpp>
#include <cubos/core/ecs/entity/entity.hpp>

using cubos::core::ecs::Entity;

template <>
void cubos::core::data::old::serialize<Entity>(Serializer& ser, const Entity& obj, const char* name)
{
    if (ser.context().has<SerializationMap<Entity, std::string>>())
    {
        if (obj.isNull())
        {
            ser.write("null", name);
            return;
        }

        auto& map = ser.context().get<SerializationMap<Entity, std::string>>();
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
    if (des.context().has<SerializationMap<Entity, std::string>>())
    {
        std::string name;
        des.read(name);
        if (name == "null")
        {
            obj = Entity();
            return;
        }

        auto& map = des.context().get<SerializationMap<Entity, std::string>>();
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
