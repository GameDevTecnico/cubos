#include <cubos/core/log.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/reflection/type_registry.hpp>

using cubos::core::reflection::Type;
using cubos::core::reflection::TypeRegistry;

void TypeRegistry::insert(const Type& type)
{
    if (mTypes.containsRight(type.name()))
    {
        CUBOS_ASSERT(mTypes.atRight(type.name()) == &type, "Two types should never have the same name");
        return;
    }

    mTypes.insert(&type, type.name());
}

void TypeRegistry::insert(const TypeRegistry& other)
{
    for (const auto& [type, _] : other)
    {
        this->insert(*type);
    }
}

bool TypeRegistry::contains(const Type& type) const
{
    return mTypes.containsLeft(&type);
}

bool TypeRegistry::contains(const std::string& name) const
{
    return mTypes.containsRight(name);
}

const Type& TypeRegistry::at(const std::string& name) const
{
    return *mTypes.atRight(name);
}

std::size_t TypeRegistry::size() const
{
    return mTypes.size();
}

auto TypeRegistry::begin() const -> Iterator
{
    return mTypes.begin();
}

auto TypeRegistry::end() const -> Iterator
{
    return mTypes.end();
}
