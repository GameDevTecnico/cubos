#include <cubos/core/reflection/dictionary.hpp>
#include <cubos/core/reflection/visitor.hpp>

using cubos::core::reflection::DictionaryType;
using cubos::core::reflection::Type;

DictionaryType::Iterator::Iterator(const DictionaryType& type, const void* dictionary)
    : mType(type)
{
    CUBOS_ASSERT(mType.mIteratorNew);
    mIterator = mType.mIteratorNew(dictionary);
}

DictionaryType::Iterator::~Iterator()
{
    if (mIterator)
    {
        CUBOS_ASSERT(mType.mIteratorDelete);
        mType.mIteratorDelete(mIterator);
    }
}

const void* DictionaryType::Iterator::key(const void* dictionary) const
{
    CUBOS_ASSERT(mIterator, "Iterator has already reached the end");
    CUBOS_ASSERT(mType.mIteratorKey);
    return mType.mIteratorKey(dictionary, mIterator);
}

const void* DictionaryType::Iterator::value(const void* dictionary) const
{
    CUBOS_ASSERT(mIterator, "Iterator has already reached the end");
    CUBOS_ASSERT(mType.mIteratorValue);
    return reinterpret_cast<const void*>(mType.mIteratorValue(dictionary, mIterator));
}

void* DictionaryType::Iterator::value(void* dictionary) const
{
    CUBOS_ASSERT(mIterator, "Iterator has already reached the end");
    CUBOS_ASSERT(mType.mIteratorValue);
    return reinterpret_cast<void*>(mType.mIteratorValue(dictionary, mIterator));
}

void DictionaryType::Iterator::increment(const void* dictionary)
{
    CUBOS_ASSERT(mIterator, "Iterator has already reached the end");
    CUBOS_ASSERT(mType.mIteratorIncrement);
    if (mType.mIteratorIncrement(dictionary, mIterator))
    {
        mIterator = nullptr;
    }
}

bool DictionaryType::Iterator::valid() const
{
    return mIterator != nullptr;
}

DictionaryType::Builder::Builder(DictionaryType& type)
    : Type::Builder(type)
    , mType(type)
{
}

DictionaryType::Builder& DictionaryType::Builder::length(Length length)
{
    mType.mLength = length;
    return *this;
}

DictionaryType::Builder& DictionaryType::Builder::value(Value value)
{
    mType.mValue = value;
    return *this;
}

DictionaryType::Builder& DictionaryType::Builder::insert(Insert insert)
{
    mType.mInsert = insert;
    return *this;
}

DictionaryType::Builder& DictionaryType::Builder::remove(Remove remove)
{
    mType.mRemove = remove;
    return *this;
}

DictionaryType::Builder& DictionaryType::Builder::clear(Clear clear)
{
    mType.mClear = clear;
    return *this;
}

DictionaryType::Builder& DictionaryType::Builder::iteratorNew(IteratorNew iteratorNew)
{
    mType.mIteratorNew = iteratorNew;
    return *this;
}

DictionaryType::Builder& DictionaryType::Builder::iteratorKey(IteratorKey iteratorKey)
{
    mType.mIteratorKey = iteratorKey;
    return *this;
}

DictionaryType::Builder& DictionaryType::Builder::iteratorValue(IteratorValue iteratorValue)
{
    mType.mIteratorValue = iteratorValue;
    return *this;
}

DictionaryType::Builder& DictionaryType::Builder::iteratorIncrement(IteratorIncrement iteratorIncrement)
{
    mType.mIteratorIncrement = iteratorIncrement;
    return *this;
}

DictionaryType::Builder& DictionaryType::Builder::iteratorDelete(IteratorDelete iteratorDelete)
{
    mType.mIteratorDelete = iteratorDelete;
    return *this;
}

DictionaryType::Builder DictionaryType::build(std::string name, const Type& keyType, const Type& valueType)
{
    return Builder{*new DictionaryType{name, keyType, valueType}};
}

const Type& DictionaryType::keyType() const
{
    return mKeyType;
}

const Type& DictionaryType::valueType() const
{
    return mValueType;
}

DictionaryType::Length DictionaryType::length() const
{
    return mLength;
}

DictionaryType::Value DictionaryType::value() const
{
    return mValue;
}

DictionaryType::Insert DictionaryType::insert() const
{
    return mInsert;
}

DictionaryType::Remove DictionaryType::remove() const
{
    return mRemove;
}

DictionaryType::Clear DictionaryType::clear() const
{
    return mClear;
}

bool DictionaryType::hasIterator() const
{
    return mIteratorNew != nullptr && mIteratorKey != nullptr && mIteratorValue != nullptr &&
           mIteratorIncrement != nullptr && mIteratorDelete != nullptr;
}

std::size_t DictionaryType::length(const void* dictionary) const
{
    CUBOS_ASSERT(mLength, "No length getter function set");
    return mLength(dictionary);
}

const void* DictionaryType::value(const void* dictionary, const void* key) const
{
    CUBOS_ASSERT(mValue, "No value getter function set");
    auto address = mValue(dictionary, key);
    if (address == 0)
    {
        return nullptr;
    }
    else
    {
        return reinterpret_cast<const void*>(address);
    }
}

void* DictionaryType::value(void* dictionary, const void* key) const
{
    CUBOS_ASSERT(mValue, "No value getter function set");
    auto address = mValue(dictionary, key);
    if (address == 0)
    {
        return nullptr;
    }
    else
    {
        return reinterpret_cast<void*>(address);
    }
}

void* DictionaryType::insert(void* dictionary, const void* key) const
{
    CUBOS_ASSERT(mInsert, "No insert function set");
    return mInsert(dictionary, key);
}

bool DictionaryType::remove(void* dictionary, const void* key) const
{
    CUBOS_ASSERT(mRemove, "No remove function set");
    return mRemove(dictionary, key);
}

void DictionaryType::clear(void* dictionary) const
{
    CUBOS_ASSERT(mClear, "No clear function set");
    mClear(dictionary);
}

DictionaryType::Iterator DictionaryType::iterator(const void* dictionary) const
{
    CUBOS_ASSERT(hasIterator(), "No iterator functions set");
    return Iterator{*this, dictionary};
}

void DictionaryType::accept(TypeVisitor& visitor) const
{
    visitor.visit(*this);
}

DictionaryType::DictionaryType(std::string name, const Type& keyType, const Type& valueType)
    : Type{std::move(name)}
    , mKeyType{keyType}
    , mValueType{valueType}
{
}
