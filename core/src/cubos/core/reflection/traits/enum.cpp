#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/enum.hpp>

using cubos::core::reflection::EnumTrait;

EnumTrait::~EnumTrait()
{
    while (mFirstVariant != nullptr)
    {
        auto* next = mFirstVariant->mNext;
        delete mFirstVariant;
        mFirstVariant = next;
    }
}

EnumTrait::EnumTrait(EnumTrait&& other) noexcept
    : mFirstVariant(other.mFirstVariant)
    , mLastVariant(other.mLastVariant)
{
    other.mFirstVariant = nullptr;
    other.mLastVariant = nullptr;
}

void EnumTrait::addVariant(std::string name, Tester tester, Setter setter)
{
    for (auto* variant = mFirstVariant; variant != nullptr; variant = variant->mNext)
    {
        CUBOS_ASSERT(variant->mName != name, "Enum variant '{}' already exists", name);
    }

    auto* variant = new Variant(std::move(name), tester, setter);
    if (mFirstVariant != nullptr)
    {
        mLastVariant->mNext = variant;
        mLastVariant = variant;
    }
    else
    {
        mFirstVariant = variant;
        mLastVariant = variant;
    }
}

EnumTrait&& EnumTrait::withVariant(std::string name, Tester tester, Setter setter) &&
{
    this->addVariant(std::move(name), tester, setter);
    return std::move(*this);
}

bool EnumTrait::contains(const std::string& name) const
{
    for (auto* variant = mFirstVariant; variant != nullptr; variant = variant->mNext)
    {
        if (variant->mName == name)
        {
            return true;
        }
    }
    return false;
}

const EnumTrait::Variant& EnumTrait::at(const std::string& name) const
{
    for (const auto* variant = mFirstVariant; variant != nullptr; variant = variant->mNext)
    {
        if (variant->mName == name)
        {
            return *variant;
        }
    }
    CUBOS_FAIL("Variant with name '{}' does not exist", name);
}

const EnumTrait::Variant& EnumTrait::variant(const void* value) const
{
    for (const auto* variant = mFirstVariant; variant != nullptr; variant = variant->mNext)
    {
        if (variant->test(value))
        {
            return *variant;
        }
    }
    CUBOS_FAIL("No registered variant covers the given value");
}

EnumTrait::Iterator EnumTrait::begin() const
{
    return Iterator{mFirstVariant};
}

EnumTrait::Iterator EnumTrait::end()
{
    return Iterator{nullptr};
}

std::size_t EnumTrait::size() const
{
    std::size_t i = 0;
    for (const auto& variant : *this)
    {
        (void)variant;
        i += 1;
    }
    return i;
}

EnumTrait::Variant::Variant(std::string name, Tester tester, Setter setter)
    : mName(std::move(name))
    , mTester(tester)
    , mSetter(setter)
{
}

bool EnumTrait::Variant::test(const void* value) const
{
    return mTester(value);
}

void EnumTrait::Variant::set(void* value) const
{
    mSetter(value);
}

const std::string& EnumTrait::Variant::name() const
{
    return mName;
}

const EnumTrait::Variant* EnumTrait::Variant::next() const
{
    return mNext;
}

EnumTrait::Iterator::Iterator(const Variant* variant)
    : mVariant{variant}
{
}

const EnumTrait::Variant& EnumTrait::Iterator::operator*() const
{
    CUBOS_ASSERT(mVariant, "Iterator does not point to a variant");
    return *mVariant;
}

const EnumTrait::Variant* EnumTrait::Iterator::operator->() const
{
    CUBOS_ASSERT(mVariant, "Iterator does not point to a variant");
    return mVariant;
}

EnumTrait::Iterator& EnumTrait::Iterator::operator++()
{
    CUBOS_ASSERT(mVariant, "Iterator does not point to a variant");
    mVariant = mVariant->next();
    return *this;
}
