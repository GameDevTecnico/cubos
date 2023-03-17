#include <cubos/core/ecs/registry.hpp>

using namespace cubos::core;
using namespace cubos::core::ecs;

bool Registry::create(const std::string& name, data::Deserializer& des, Blueprint& blueprint, Entity id)
{
    auto& creators = Registry::creators();

    if (creators.find(name) == creators.end())
    {
        CUBOS_ERROR("Component type '{}' not registered", name);
        return false;
    }

    return creators.at(name)(des, blueprint, id);
}

const std::string& Registry::name(std::type_index index)
{
    auto& names = Registry::names();
    if (auto it = names.at(index))
    {
        return *it;
    }

    CUBOS_CRITICAL("Component type '{}' not registered", index.name());
    abort();
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

memory::TypeMap<std::string>& Registry::names()
{
    static memory::TypeMap<std::string> names;
    return names;
}
