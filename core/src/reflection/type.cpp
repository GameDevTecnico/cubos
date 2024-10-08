#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/tel/logging.hpp>

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
    // Remove all alphanumeric characters preceding colons in the name.
    bool isNamespace{false};
    for (int i = static_cast<int>(mName.size()) - 1; i >= 0; --i)
    {
        auto c = mName[static_cast<std::size_t>(i)];
        if (c == ':')
        {
            isNamespace = true;
            continue;
        }

        if (std::isalnum(c) == 0)
        {
            isNamespace = false;
        }

        if (!isNamespace)
        {
            mShortName.push_back(c);
        }
    }
    mShortName = std::string(mShortName.rbegin(), mShortName.rend());
}
