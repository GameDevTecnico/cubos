#include <cubos/core/ecs/registry.hpp>

using namespace cubos::core;
using namespace cubos::core::ecs;

bool Registry::create(const std::string& name, data::Deserializer& des, Blueprint& blueprint,
                      const data::SerializationMap<Entity, std::string>& map, data::Handle::DesContext handleCtx,
                      Entity id)
{
    auto& creators = Registry::creators();

    if (creators.find(name) == creators.end())
    {
        CUBOS_ERROR("Component type '{}' not registered", name);
        return false;
    }

    return creators.at(name)(des, blueprint, map, handleCtx, id);
}

const std::string& Registry::name(std::type_index index)
{
    auto& names = Registry::names();
    auto it = names.find(index);
    if (it == names.end())
    {
        CUBOS_CRITICAL("Component type '{}' not registered", index.name());
        abort();
    }
    return it->second;
}

std::type_index Registry::index(const std::string& name)
{
    auto& names = Registry::names();
    for (const auto& pair : names)
    {
        if (pair.second == name)
        {
            return pair.first;
        }
    }

    return typeid(void);
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
