#include <cubos/core/ecs/debug.hpp>

using namespace cubos::core;
using namespace cubos::core::ecs;

Debug::Iterator::Iterator(const World& world, EntityManager::Iterator it) : world(world), it(it)
{
    // Do nothing.
}

std::tuple<Entity, data::Package> Debug::Iterator::operator*() const
{
    return {*it, this->world.pack(*it, [](data::Serializer& ser, const data::Handle& handle, const char* name) {
                ser.write(handle.getId(), name);
            })};
}

bool Debug::Iterator::operator==(const Iterator& other) const
{
    return it == other.it;
}

bool Debug::Iterator::operator!=(const Iterator& other) const
{
    return it != other.it;
}

Debug::Iterator& Debug::Iterator::operator++()
{
    ++it;
    return *this;
}

Debug::Debug(World& world) : world(world)
{
    // Do nothing.
}

Debug::Iterator Debug::begin()
{
    return Iterator(this->world, this->world.entityManager.begin());
}

Debug::Iterator Debug::end()
{
    return Iterator(this->world, this->world.entityManager.end());
}

data::Package Debug::pack(Entity entity)
{
    return this->world.pack(entity, [](data::Serializer& ser, const data::Handle& handle, const char* name) {
        ser.write(handle.getId(), name);
    });
}

void Debug::unpack(Entity entity, const data::Package& package, data::Handle::DesContext handleCtx)
{
    this->world.unpack(entity, package, handleCtx);
}
