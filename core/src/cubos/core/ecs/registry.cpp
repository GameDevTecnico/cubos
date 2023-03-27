#include <cubos/core/ecs/registry.hpp>

using namespace cubos::core;
using namespace cubos::core::ecs;

bool Registry::create(std::string_view name, data::Deserializer& des, Blueprint& blueprint, Entity id)
{
    auto& creators = Registry::entriesByName();
    if (auto it = creators.find(std::string(name)); it != creators.end())
    {
        return it->second->componentCreator(des, blueprint, id);
    }

    CUBOS_ERROR("Component with name '{}' not registered", name);
    return false;
}

std::unique_ptr<IStorage> Registry::createStorage(std::type_index type)
{
    auto& creators = Registry::entriesByType();
    if (auto it = creators.at(type))
    {
        return (*it)->storageCreator();
    }

    CUBOS_ERROR("Component type '{}' not registered", type.name());
    return nullptr;
}

std::optional<std::string_view> Registry::name(std::type_index type)
{
    auto& entries = Registry::entriesByType();
    if (auto it = entries.at(type))
    {
        return (*it)->name;
    }

    CUBOS_ERROR("Component type '{}' not registered", type.name());
    return std::nullopt;
}

std::optional<std::type_index> Registry::type(std::string_view name)
{
    auto& entries = Registry::entriesByName();
    if (auto it = entries.find(std::string(name)); it != entries.end())
    {
        return it->second->type;
    }

    CUBOS_ERROR("Component with name '{}' not registered", name);
    return std::nullopt;
}

memory::TypeMap<std::shared_ptr<Registry::Entry>>& Registry::entriesByType()
{
    static memory::TypeMap<std::shared_ptr<Entry>> entries;
    return entries;
}

std::unordered_map<std::string, std::shared_ptr<Registry::Entry>>& Registry::entriesByName()
{
    static std::unordered_map<std::string, std::shared_ptr<Entry>> entries;
    return entries;
}
