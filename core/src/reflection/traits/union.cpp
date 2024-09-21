#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/union.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::UnionTrait;

CUBOS_REFLECT_IMPL(UnionTrait)
{
    return Type::create("cubos::core::ecs::UnionTrait");
}

UnionTrait::~UnionTrait()
{
    while (mFirstVariant != nullptr)
    {
        auto* next = mFirstVariant->mNext;
        delete mFirstVariant;
        mFirstVariant = next;
    }
}

UnionTrait::UnionTrait(UnionTrait&& other) noexcept
    : mFirstVariant(other.mFirstVariant)
    , mLastVariant(other.mLastVariant)
{
    other.mFirstVariant = nullptr;
    other.mLastVariant = nullptr;
}

void UnionTrait::addVariant(std::string name, const Type& type, Tester tester, Setter setter, Getter getter)
{
    for (auto* variant = mFirstVariant; variant != nullptr; variant = variant->mNext)
    {
        CUBOS_ASSERT(variant->mName != name, "Union variant {} already exists", name);
    }

    auto* variant = new Variant(std::move(name), type, tester, setter, getter);
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

UnionTrait&& UnionTrait::withVariant(std::string name, const Type& type, Tester tester, Setter setter, Getter getter) &&
{
    this->addVariant(std::move(name), type, tester, setter, getter);
    return std::move(*this);
}

bool UnionTrait::contains(const std::string& name) const
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

bool UnionTrait::contains(const Type& type) const
{
    for (auto* variant = mFirstVariant; variant != nullptr; variant = variant->mNext)
    {
        if (variant->mType == type)
        {
            return true;
        }
    }
    return false;
}

const UnionTrait::Variant& UnionTrait::at(const std::string& name) const
{
    for (const auto* variant = mFirstVariant; variant != nullptr; variant = variant->mNext)
    {
        if (variant->mName == name)
        {
            return *variant;
        }
    }
    CUBOS_FAIL("Union with name {} does not exist", name);
}

const UnionTrait::Variant& UnionTrait::at(const Type& type) const
{
    for (const auto* variant = mFirstVariant; variant != nullptr; variant = variant->mNext)
    {
        if (variant->mType == type)
        {
            return *variant;
        }
    }
    CUBOS_FAIL("Union with type {} does not exist", type.name());
}

const UnionTrait::Variant& UnionTrait::variant(const void* value) const
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

UnionTrait::Iterator UnionTrait::begin() const
{
    return Iterator{mFirstVariant};
}

UnionTrait::Iterator UnionTrait::end()
{
    return Iterator{nullptr};
}

std::size_t UnionTrait::size() const
{
    std::size_t i = 0;
    for (const auto& variant : *this)
    {
        (void)variant;
        i += 1;
    }
    return i;
}

UnionTrait::Variant::Variant(std::string name, const Type& type, Tester tester, Setter setter, Getter getter)
    : mName(std::move(name))
    , mType(type)
    , mTester(tester)
    , mSetter(setter)
    , mGetter(getter)
{
}

bool UnionTrait::Variant::test(const void* value) const
{
    return mTester(value);
}

void UnionTrait::Variant::set(void* value) const
{
    mSetter(value);
}

const void* UnionTrait::Variant::get(const void* value) const
{
    return reinterpret_cast<const void*>(mGetter(value));
}

void* UnionTrait::Variant::get(void* value) const
{
    return reinterpret_cast<void*>(mGetter(value));
}

const std::string& UnionTrait::Variant::name() const
{
    return mName;
}

const UnionTrait::Variant* UnionTrait::Variant::next() const
{
    return mNext;
}

UnionTrait::Iterator::Iterator(const Variant* variant)
    : mVariant{variant}
{
}

const UnionTrait::Variant& UnionTrait::Iterator::operator*() const
{
    CUBOS_ASSERT(mVariant, "Iterator does not point to a variant");
    return *mVariant;
}

const UnionTrait::Variant* UnionTrait::Iterator::operator->() const
{
    CUBOS_ASSERT(mVariant, "Iterator does not point to a variant");
    return mVariant;
}

UnionTrait::Iterator& UnionTrait::Iterator::operator++()
{
    CUBOS_ASSERT(mVariant, "Iterator does not point to a variant");
    mVariant = mVariant->next();
    return *this;
}
