#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/ecs/registry.hpp>

using namespace cubos::core::ecs;

Blueprint::~Blueprint()
{
    for (const auto& buffer : this->buffers)
    {
        delete buffer.second;
    }
}

bool Blueprint::addFromDeserializer(Entity entity, const std::string& name, data::Deserializer& deserializer)
{
    assert(entity.generation == 0);
    return Registry::create(name, deserializer, *this, entity);
}

Entity Blueprint::entity(const std::string& name) const
{
    if (!this->map.hasId(name))
    {
        return {};
    }

    return this->map.getRef(name);
}

void Blueprint::merge(const std::string& prefix, const Blueprint& other)
{
    // First, merge the maps.
    data::SerializationMap<Entity, std::string> dstMap;
    for (uint32_t i = 0; i < static_cast<uint32_t>(other.map.size()); ++i)
    {
        auto name = prefix + "." + other.map.getId(Entity(i, 0));
        this->map.add(Entity(static_cast<uint32_t>(this->map.size()), 0), name);
        dstMap.add(Entity(i, 0), name);
    }

    data::Context src;
    data::Context dst;
    src.push(this->map);
    dst.push(dstMap);

    /// Then, merge the buffers.
    for (const auto& buffer : other.buffers)
    {
        auto* ptr = this->buffers.at(buffer.first);
        IBuffer* buf;
        if (ptr == nullptr)
        {
            buf = buffer.second->create();
            this->buffers.set(buffer.first, buf);
        }
        else
        {
            buf = *ptr;
        }

        buf->merge(buffer.second, prefix, src, dst);
    }
}

void Blueprint::clear()
{
    this->map.clear();
    for (const auto& buffer : this->buffers)
    {
        delete buffer.second;
    }
    this->buffers.clear();
}
