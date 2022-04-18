#include <cubos/engine/data/meta.hpp>

using namespace cubos;
using namespace cubos::engine::data;

Meta::Meta(Meta&& rhs)
{
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

void Meta::serialize(core::memory::Serializer& serializer) const
{
    serializer.write(this->id, "id");
    serializer.write(this->type, "type");
    switch (this->usage)
    {
    case Usage::Static:
        serializer.write("Static", "usage");
        break;
    case Usage::Dynamic:
        serializer.write("Dynamic", "usage");
        break;
    }
    serializer.write(this->parameters, "params");
}

void Meta::deserialize(core::memory::Deserializer& deserializer)
{
    std::string usage;
    deserializer.read(this->id);
    deserializer.read(this->type);
    deserializer.read(usage);
    deserializer.read(this->parameters);

    if (usage == "Static")
    {
        this->usage = Usage::Static;
    }
    else if (usage == "Dynamic")
    {
        this->usage = Usage::Dynamic;
    }
    else
    {
        core::logError("asset::Meta::deserialize() failed: Invalid usage type '{}'", usage);
        abort();
    }
}
