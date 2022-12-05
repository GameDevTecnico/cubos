#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/ecs/registry.hpp>

using namespace cubos::core::ecs;

Blueprint::~Blueprint()
{
    for (auto& buffer : this->buffers)
    {
        delete buffer.second;
    }
}

bool Blueprint::addFromDeserializer(Entity entity, const std::string& name, data::Deserializer& deserializer,
                                    data::Handle::DesContext handleCtx)
{
    assert(entity.generation == 0);
    return Registry::create(name, deserializer, *this, this->map, handleCtx, entity);
}

Entity Blueprint::entity(const std::string& name) const
{
    if (!this->map.hasId(name))
    {
        return Entity();
    }

    return this->map.getRef(name);
}

void Blueprint::merge(const std::string& prefix, const Blueprint& other)
{
    // First, merge the maps.
    data::SerializationMap<Entity, std::string> dstMap;
    auto oldEntityCount = static_cast<uint32_t>(this->map.size());
    for (uint32_t i = 0; i < static_cast<uint32_t>(other.map.size()); ++i)
    {
        auto name = prefix + "." + other.map.getId(Entity(i, 0));
        this->map.add(Entity(static_cast<uint32_t>(this->map.size()), 0), name);
        dstMap.add(Entity(i, 0), name);
    }

    auto serHandleCtx = [&](data::Serializer& ser, const data::Handle& handle, const char* name) {
        auto id = reinterpret_cast<uint64_t>(handle.getRaw());
        ser.write(id, name);
        this->handles.emplace(id, handle);
    };

    auto desHandleCtx = [&](data::Deserializer& des, data::Handle& handle) {
        uint64_t id;
        des.read(id);
        handle = this->handles[id];
    };

    /// Then, merge the buffers.
    for (auto& buffer : other.buffers)
    {
        auto it = this->buffers.find(buffer.first);
        if (it == this->buffers.end())
        {
            it = this->buffers.emplace(buffer.first, buffer.second->create()).first;
        }

        it->second->merge(buffer.second, prefix, other.map, desHandleCtx, dstMap, serHandleCtx);
    }
}

void Blueprint::clear()
{
    this->map.clear();
    for (auto& buffer : this->buffers)
    {
        delete buffer.second;
    }
    this->buffers.clear();
}
