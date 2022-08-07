#include <cubos/core/ecs/registry.hpp>

using namespace cubos::core;
using namespace cubos::core::ecs;

bool Registry::create(const std::string& name, data::Deserializer& des, Blueprint& blueprint,
                      data::SerializationMap<Entity, std::string>* map, Entity id)
{
    auto& creators = Registry::creators();

    if (creators.find(name) == creators.end())
    {
        core::logWarning("Registry::create(): component type {} not registered", name);
        return false;
    }

    return creators.at(name)(des, blueprint, map, id);
}

void Registry::add(std::type_index index, const std::string& name, Creator fun)
{
    auto& creators = Registry::creators();
    auto& names = Registry::names();
    assert(creators.find(name) == creators.end());
    creators.emplace(name, fun);
    names.emplace(index, name);
}

const std::string& Registry::name(std::type_index index)
{
    auto& names = Registry::names();
    auto it = names.find(index);
    if (it == names.end())
    {
        core::logCritical("Registry::name(): component type {} not registered", index.name());
        abort();
    }
    return it->second;
}

std::unordered_map<std::string, Registry::Creator>& Registry::creators()
{
    static std::unordered_map<std::string, Creator> creators;
    return creators;
}

std::unordered_map<std::type_index, std::string>& Registry::names()
{
    static std::unordered_map<std::type_index, std::string> names;
    return names;
}
