#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/ecs/registry.hpp>

using namespace cubos::core::ecs;

Blueprint::~Blueprint()
{
    for (const auto& buffer : mBuffers)
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
    if (!mMap.hasId(name))
    {
        return {};
    }

    return mMap.getRef(name);
}

void Blueprint::merge(const std::string& prefix, const Blueprint& other)
{
    // First, merge the maps.
    data::SerializationMap<Entity, std::string> dstMap;
    for (uint32_t i = 0; i < static_cast<uint32_t>(other.mMap.size()); ++i)
    {
        auto name = prefix + "." + other.mMap.getId(Entity(i, 0));
        mMap.add(Entity(static_cast<uint32_t>(mMap.size()), 0), name);
        dstMap.add(Entity(i, 0), name);
    }

    data::Context src;
    data::Context dst;
    src.push(mMap);
    dst.push(dstMap);

    /// Then, merge the buffers.
    for (const auto& buffer : other.mBuffers)
    {
        auto* ptr = mBuffers.at(buffer.first);
        IBuffer* buf;
        if (ptr == nullptr)
        {
            buf = buffer.second->create();
            mBuffers.set(buffer.first, buf);
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
    mMap.clear();
    for (const auto& buffer : mBuffers)
    {
        delete buffer.second;
    }
    mBuffers.clear();
}
