#include <new>

#include <cubos/core/log.hpp>
#include <cubos/core/memory/any_value.hpp>
#include <cubos/core/memory/move.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::memory::AnyValue;
using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::Type;

AnyValue::~AnyValue()
{
    // Might have been moved, we must check it here.
    if (mValue != nullptr)
    {
        CUBOS_ASSERT(mType != nullptr, "Type must not be null");
        const auto& trait = mType->get<ConstructibleTrait>();
        trait.destruct(mValue);
        operator delete(mValue, static_cast<std::align_val_t>(trait.alignment()));
    }
}

AnyValue::AnyValue()
    : mType(nullptr)
    , mValue(nullptr)
{
}

AnyValue& AnyValue::operator=(AnyValue&& other) noexcept
{
    mType = other.mType;
    mValue = other.mValue;
    other.mType = nullptr;
    other.mValue = nullptr;
    return *this;
}

AnyValue::AnyValue(AnyValue&& other) noexcept
    : mType(other.mType)
    , mValue(other.mValue)
{
    other.mType = nullptr;
    other.mValue = nullptr;
}

AnyValue::AnyValue(const AnyValue& other) noexcept
    : AnyValue{*other.mType}
{
    CUBOS_ASSERT(mType->has<ConstructibleTrait>(), "Type must be constructible");
    const auto& trait = mType->get<ConstructibleTrait>();
    CUBOS_ASSERT(trait.hasCopyConstruct(), "Type must be copy constructible");

    trait.copyConstruct(get(), other.get());
}

AnyValue AnyValue::defaultConstruct(const Type& type) noexcept
{
    CUBOS_ASSERT(type.has<ConstructibleTrait>(), "Type must be constructible");
    const auto& trait = type.get<ConstructibleTrait>();
    CUBOS_ASSERT(trait.hasDefaultConstruct(), "Type must be default constructible");

    AnyValue any{type};
    trait.defaultConstruct(any.get());
    return move(any);
}

AnyValue AnyValue::copyConstruct(const Type& type, const void* value) noexcept
{
    CUBOS_ASSERT(type.has<ConstructibleTrait>(), "Type must be constructible");
    const auto& trait = type.get<ConstructibleTrait>();
    CUBOS_ASSERT(trait.hasCopyConstruct(), "Type must be copy constructible");

    AnyValue any{type};
    trait.copyConstruct(any.get(), value);
    return move(any);
}

AnyValue AnyValue::moveConstruct(const Type& type, void* value) noexcept
{
    CUBOS_ASSERT(type.has<ConstructibleTrait>(), "Type must be constructible");
    const auto& trait = type.get<ConstructibleTrait>();
    CUBOS_ASSERT(trait.hasMoveConstruct(), "Type must be move constructible");

    AnyValue any{type};
    trait.moveConstruct(any.get(), value);
    return move(any);
}

const Type& AnyValue::type() const
{
    CUBOS_ASSERT(this->valid(), "This must hold a value");
    return *mType;
}

void* AnyValue::get()
{
    CUBOS_ASSERT(this->valid(), "This must hold a value");
    return mValue;
}

const void* AnyValue::get() const
{
    CUBOS_ASSERT(this->valid(), "This must hold a value");
    return mValue;
}

bool AnyValue::valid() const
{
    return mType != nullptr;
}

AnyValue::AnyValue(const Type& type) noexcept
    : mType(&type)
{
    const auto& trait = type.get<ConstructibleTrait>();
    mValue = operator new(trait.size(), static_cast<std::align_val_t>(trait.alignment()), std::nothrow);

    CUBOS_ASSERT(mValue != nullptr, "Could not allocate memory for value of type {}", type.name());
}
