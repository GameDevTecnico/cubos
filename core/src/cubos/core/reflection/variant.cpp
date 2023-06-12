#include <cubos/core/reflection/variant.hpp>
#include <cubos/core/reflection/visitor.hpp>

using cubos::core::reflection::Type;
using cubos::core::reflection::VariantType;

VariantType::Variant::Variant(const Type& type, Address address, EmplaceDefault emplaceDefault)
    : mType(type)
    , mAddress(address)
    , mEmplaceDefault(emplaceDefault)
{
}

const Type& VariantType::Variant::type() const
{
    return mType;
}

VariantType::Variant::Address VariantType::Variant::address() const
{
    return mAddress;
}

VariantType::Variant::EmplaceDefault VariantType::Variant::emplaceDefault() const
{
    return mEmplaceDefault;
}

const void* VariantType::Variant::get(const void* instance) const
{
    CUBOS_ASSERT(mAddress, "No address function set");
    return reinterpret_cast<const void*>(mAddress(instance));
}

void* VariantType::Variant::get(void* instance) const
{
    CUBOS_ASSERT(mAddress, "No address function set");
    return reinterpret_cast<void*>(mAddress(instance));
}

void VariantType::Variant::emplaceDefault(void* instance) const
{
    CUBOS_ASSERT(mEmplaceDefault, "No emplace default function set");
    mEmplaceDefault(instance);
}

VariantType::Builder::Builder(VariantType& type)
    : Type::Builder(type)
    , mType(type)
    , mAddress(nullptr)
    , mEmplaceDefault(nullptr)
{
}

VariantType::Builder& VariantType::Builder::index(Index index)
{
    mType.mIndex = index;
    return *this;
}

VariantType::Builder& VariantType::Builder::address(Variant::Address address)
{
    CUBOS_ASSERT(mAddress == nullptr, "Address function already set");
    mAddress = address;
    return *this;
}

VariantType::Builder& VariantType::Builder::emplaceDefault(Variant::EmplaceDefault emplaceDefault)
{
    CUBOS_ASSERT(mEmplaceDefault == nullptr, "Emplace default function already set");
    mEmplaceDefault = emplaceDefault;
    return *this;
}

VariantType::Builder& VariantType::Builder::variant(const Type& type)
{
    mType.mVariants.emplace_back(type, mAddress, mEmplaceDefault);
    mAddress = nullptr;
    mEmplaceDefault = nullptr;
    return *this;
}

VariantType::Builder VariantType::builder(std::string name)
{
    return Builder{*new VariantType(std::move(name))};
}

VariantType::Index VariantType::index() const
{
    return mIndex;
}

const std::vector<VariantType::Variant>& VariantType::variants() const
{
    return mVariants;
}

const VariantType::Variant& VariantType::variant(const Type& type) const
{
    for (const auto& variant : mVariants)
    {
        if (&variant.type() == &type)
        {
            return variant;
        }
    }

    CUBOS_FAIL("Type not found");
}

const VariantType::Variant& VariantType::variant(const void* instance) const
{
    CUBOS_ASSERT(mIndex, "No index function set");
    return mVariants[mIndex(instance)];
}

std::size_t VariantType::index(const void* instance) const
{
    CUBOS_ASSERT(mIndex, "No index function set");
    return mIndex(instance);
}

void VariantType::accept(TypeVisitor& visitor) const
{
    visitor.visit(*this);
}

VariantType::VariantType(std::string name)
    : Type(std::move(name))
    , mIndex(nullptr)
{
}
