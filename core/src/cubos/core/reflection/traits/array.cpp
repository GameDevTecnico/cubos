#include <cubos/core/log.hpp>
#include <cubos/core/reflection/traits/array.hpp>

using cubos::core::reflection::ArrayTrait;
using cubos::core::reflection::Type;

ArrayTrait::ArrayTrait(const Type& elementType, Length length, AddressOf addressOf)
    : mElementType(elementType)
    , mLength(length)
    , mAddressOf(addressOf)
    , mInsertDefault(nullptr)
    , mInsertCopy(nullptr)
    , mInsertMove(nullptr)
    , mErase(nullptr)
{
}

void ArrayTrait::setInsertDefault(InsertDefault insertDefault)
{
    CUBOS_ASSERT(!mInsertDefault, "Insert default already set");
    mInsertDefault = insertDefault;
}

void ArrayTrait::setInsertCopy(InsertCopy insertCopy)
{
    CUBOS_ASSERT(!mInsertCopy, "Insert copy already set");
    mInsertCopy = insertCopy;
}

void ArrayTrait::setInsertMove(InsertMove insertMove)
{
    CUBOS_ASSERT(!mInsertMove, "Insert move already set");
    mInsertMove = insertMove;
}

void ArrayTrait::setErase(Erase erase)
{
    CUBOS_ASSERT(!mErase, "Erase already set");
    mErase = erase;
}

const Type& ArrayTrait::elementType() const
{
    return mElementType;
}

std::size_t ArrayTrait::length(const void* instance) const
{
    return mLength(instance);
}

void* ArrayTrait::get(void* instance, std::size_t index) const
{
    return reinterpret_cast<void*>(mAddressOf(instance, index));
}

const void* ArrayTrait::get(const void* instance, std::size_t index) const
{
    return reinterpret_cast<const void*>(mAddressOf(instance, index));
}

bool ArrayTrait::insertDefault(void* instance, std::size_t index) const
{
    if (mInsertDefault != nullptr)
    {
        mInsertDefault(instance, index);
        return true;
    }

    return false;
}

bool ArrayTrait::insertCopy(void* instance, std::size_t index, const void* value) const
{
    if (mInsertCopy != nullptr)
    {
        mInsertCopy(instance, index, value);
        return true;
    }

    return false;
}

bool ArrayTrait::insertMove(void* instance, std::size_t index, void* value) const
{
    if (mInsertMove != nullptr)
    {
        mInsertMove(instance, index, value);
        return true;
    }

    return false;
}

bool ArrayTrait::erase(void* instance, std::size_t index) const
{
    if (mErase != nullptr)
    {
        mErase(instance, index);
        return true;
    }

    return false;
}

bool ArrayTrait::hasInsertDefault() const
{
    return mInsertDefault != nullptr;
}

bool ArrayTrait::hasInsertCopy() const
{
    return mInsertCopy != nullptr;
}

bool ArrayTrait::hasInsertMove() const
{
    return mInsertMove != nullptr;
}

bool ArrayTrait::hasErase() const
{
    return mErase != nullptr;
}
