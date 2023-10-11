#include <new>

#include <cubos/core/log.hpp>
#include <cubos/core/memory/any_vector.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::memory::AnyVector;
using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::Type;

AnyVector::~AnyVector()
{
    operator delete(mData, static_cast<std::align_val_t>(mConstructibleTrait->alignment()));
}

AnyVector::AnyVector(const Type& elementType)
    : mElementType(elementType)
{
    CUBOS_ASSERT(mElementType.has<ConstructibleTrait>(), "Type must be constructible");
    mConstructibleTrait = &mElementType.get<ConstructibleTrait>();
    CUBOS_ASSERT(mConstructibleTrait->hasMoveConstruct(), "Type must be move constructible");

    CUBOS_ASSERT(mConstructibleTrait->size() % mConstructibleTrait->alignment() == 0,
                 "Size must be a multiple of alignment");
    mStride = mConstructibleTrait->size();
}

AnyVector::AnyVector(AnyVector&& other) noexcept
    : mElementType(other.mElementType)
    , mConstructibleTrait(other.mConstructibleTrait)
    , mData(other.mData)
    , mSize(other.mSize)
    , mCapacity(other.mCapacity)
    , mStride(other.mStride)
{
    other.mSize = 0;
    other.mCapacity = 0;
    other.mData = nullptr;
}

const Type& AnyVector::elementType() const
{
    return mElementType;
}

void AnyVector::reserve(std::size_t capacity)
{
    CUBOS_ASSERT(capacity >= mSize, "Capacity must be greater than or equal to size");

    if (mCapacity == capacity)
    {
        return;
    }

    // Allocate a new buffer, move the values there and then free the old one.
    void* data = operator new(capacity* mStride, static_cast<std::align_val_t>(mConstructibleTrait->alignment()),
                              std::nothrow);
    CUBOS_ASSERT(data != nullptr, "Vector memory allocation failed");
    for (std::size_t i = 0; i < mSize; ++i)
    {
        mConstructibleTrait->moveConstruct(static_cast<char*>(data) + i * mStride,
                                           static_cast<char*>(mData) + i * mStride);
    }
    operator delete(mData, static_cast<std::align_val_t>(mConstructibleTrait->alignment()));

    mData = data;
    mCapacity = capacity;
}

void AnyVector::pushDefault()
{
    CUBOS_ASSERT(mConstructibleTrait->hasDefaultConstruct(), "Type must be default-constructible");

    if (mSize == mCapacity)
    {
        this->reserve(mCapacity == 0 ? 1 : mCapacity * 2);
    }

    mConstructibleTrait->defaultConstruct(static_cast<char*>(mData) + mStride * mSize);
    ++mSize;
}

void AnyVector::pushCopy(const void* value)
{
    CUBOS_ASSERT(mConstructibleTrait->hasCopyConstruct(), "Type must be copy-constructible");

    if (mSize == mCapacity)
    {
        this->reserve(mCapacity == 0 ? 1 : mCapacity * 2);
    }

    mConstructibleTrait->copyConstruct(static_cast<char*>(mData) + mStride * mSize, value);
    ++mSize;
}

void AnyVector::pushMove(void* value)
{
    CUBOS_ASSERT(mConstructibleTrait->hasMoveConstruct(), "Type must be move-constructible");

    if (mSize == mCapacity)
    {
        this->reserve(mCapacity == 0 ? 1 : mCapacity * 2);
    }

    mConstructibleTrait->moveConstruct(static_cast<char*>(mData) + mStride * mSize, value);
    ++mSize;
}

void AnyVector::pop()
{
    CUBOS_ASSERT(mSize > 0, "Vector must not be empty");

    --mSize;
    mConstructibleTrait->destruct(static_cast<char*>(mData) + mStride * mSize);
}

void AnyVector::clear()
{
    for (std::size_t i = 0; i < mSize; ++i)
    {
        mConstructibleTrait->destruct(static_cast<char*>(mData) + mStride * i);
    }

    mSize = 0;
}

void* AnyVector::at(std::size_t index)
{
    CUBOS_ASSERT(index < mSize, "Index must be less than size");
    return static_cast<char*>(mData) + mStride * index;
}

const void* AnyVector::at(std::size_t index) const
{
    CUBOS_ASSERT(index < mSize, "Index must be less than size");
    return static_cast<char*>(mData) + mStride * index;
}

std::size_t AnyVector::size() const
{
    return mSize;
}

std::size_t AnyVector::capacity() const
{
    return mCapacity;
}

bool AnyVector::empty() const
{
    return mSize == 0;
}
