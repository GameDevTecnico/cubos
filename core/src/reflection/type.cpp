#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/type.hpp>

using namespace cubos::core::reflection;

Type& Type::create(std::string name)
{
    return *(new Type(std::move(name)));
}

Type& Type::unnamed(unsigned long long id)
{
    auto& type = Type::create("unnamed" + std::to_string(id));
    type.mImplemented = false;
    return type;
}

void Type::destroy(Type& type)
{
    delete &type;
}

const std::string& Type::name() const
{
    return mName;
}

const std::string& Type::shortName() const
{
    return mShortName;
}

Type& Type::with(const Type& type, void* trait, void (*deleter)(void*))
{
    CUBOS_ASSERT(!this->has(type), "Trait {} already present in type {}", type.name(), this->name());

    mTraits.emplace_back(Trait{
        .type = type,
        .value = trait,
        .deleter = deleter,
    });

    return *this;
}

bool Type::has(const Type& type) const
{
    for (const auto& trait : mTraits)
    {
        if (trait.type == type)
        {
            return true;
        }
    }

    return false;
}

const void* Type::get(const Type& type) const
{
    for (const auto& trait : mTraits)
    {
        if (trait.type == type)
        {
            return trait.value;
        }
    }

    CUBOS_FAIL("No such trait {} in type {}", type.name(), this->name());
}

bool Type::operator==(const Type& other) const
{
    if (this == &other)
    {
        return true;
    }

    CUBOS_ASSERT(this->mName != other.mName, "Two types should never have the same name");
    return false;
}

bool Type::implemented() const
{
    return mImplemented;
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
    , mShortName(mName.substr(mName.find_last_of("::") + 1))
{
}
