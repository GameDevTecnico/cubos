#include <cubos/core/log.hpp>
#include <cubos/core/reflection/type.hpp>

using namespace cubos::core::reflection;

Type& Type::create(std::string name)
{
    return *(new Type(std::move(name)));
}

void Type::destroy(Type& type)
{
    delete &type;
}

const std::string& Type::name() const
{
    return mName;
}

Type& Type::with(uintptr_t id, void* trait, void (*deleter)(void*))
{
    CUBOS_ASSERT(!this->has(id), "Trait already present in type \"{}\"", this->name());

    mTraits.emplace_back(Trait{
        .id = id,
        .value = trait,
        .deleter = deleter,
    });

    return *this;
}

bool Type::has(uintptr_t id) const
{
    for (const auto& trait : mTraits)
    {
        if (trait.id == id)
        {
            return true;
        }
    }

    return false;
}

const void* Type::get(uintptr_t id) const
{
    for (const auto& trait : mTraits)
    {
        if (trait.id == id)
        {
            return trait.value;
        }
    }

    CUBOS_FAIL("No such trait in type \"{}\"", this->name());
}

bool Type::operator==(const Type& other) const
{
    if (this == &other)
    {
        return true;
    }

    CUBOS_ASSERT(this->mName != other.mName, "Two types cannot be created with the same");
    return false;
}

Type::~Type()
{
    for (auto& trait : mTraits)
    {
        trait.deleter(trait.value);
    }
}

Type::Type(std::string name)
    : mName(std::move(name))
{
}
