#include <cubos/engine/data/meta.hpp>

using namespace cubos;
using namespace cubos::engine::data;

Meta::Meta(const std::string& id, const std::string& type, Usage usage) : id(id), type(type), usage(usage)
{
    this->stored = true;
}

Meta::Meta(Meta&& rhs)
{
    this->stored = rhs.stored;
    this->id = std::move(rhs.id);
    this->type = std::move(rhs.type);
    this->usage = rhs.usage;
    this->parameters = std::move(rhs.parameters);
}

const std::string& Meta::getId() const
{
    return this->id;
}

const std::string& Meta::getType() const
{
    return this->type;
}

Usage Meta::getUsage() const
{
    return this->usage;
}

const std::unordered_map<std::string, std::string>& Meta::getParameters() const
{
    return this->parameters;
}

void cubos::core::data::serialize(Serializer& serializer, const Meta& meta, const char* name)
{
    serializer.beginObject(name);
    serializer.write(meta.id, "id");
    serializer.write(meta.type, "type");
    switch (meta.usage)
    {
    case Usage::Static:
        serializer.write("Static", "usage");
        break;
    case Usage::Dynamic:
        serializer.write("Dynamic", "usage");
        break;
    }
    serializer.write(meta.parameters, "params");
    serializer.endObject();
}

void cubos::core::data::deserialize(Deserializer& deserializer, Meta& meta)
{
    meta.stored = false;

    std::string usage;
    deserializer.beginObject();
    deserializer.read(meta.id);
    deserializer.read(meta.type);
    deserializer.read(usage);
    deserializer.read(meta.parameters);
    deserializer.endObject();

    if (usage == "Static")
    {
        meta.usage = Usage::Static;
    }
    else if (usage == "Dynamic")
    {
        meta.usage = Usage::Dynamic;
    }
    else
    {
        CUBOS_ERROR("Invalid asset usage type '{}', defaulting to Usage::Static", usage);
        meta.usage = Usage::Static;
    }
}
