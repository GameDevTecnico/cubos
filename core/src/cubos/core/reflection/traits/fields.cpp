#include <cubos/core/log.hpp>
#include <cubos/core/reflection/traits/fields.hpp>

using namespace cubos::core::reflection;

FieldsTrait::Field::~Field()
{
    delete mAddressOf;
}

FieldsTrait::Field::Field(const Type& type, std::string name, AddressOf* addressOf)
    : mType(type)
    , mName(std::move(name))
    , mAddressOf(addressOf)
{
    CUBOS_ASSERT(mAddressOf, "addressOf must not be null");
}

const Type& FieldsTrait::Field::type() const
{
    return mType;
}

const std::string& FieldsTrait::Field::name() const
{
    return mName;
}

uintptr_t FieldsTrait::Field::addressOf(const void* instance) const
{
    return mAddressOf->get(instance);
}

void* FieldsTrait::Field::pointerTo(void* instance) const
{
    return reinterpret_cast<void*>(this->addressOf(instance));
}

const void* FieldsTrait::Field::pointerTo(const void* instance) const
{
    return reinterpret_cast<const void*>(this->addressOf(instance));
}

const FieldsTrait::Field* FieldsTrait::Field::next() const
{
    return mNext;
}

FieldsTrait::Iterator::Iterator(const Field* field)
    : mField{field}
{
}

const FieldsTrait::Field& FieldsTrait::Iterator::operator*() const
{
    CUBOS_ASSERT(mField, "Iterator does not point to a field");
    return *mField;
}

const FieldsTrait::Field* FieldsTrait::Iterator::operator->() const
{
    CUBOS_ASSERT(mField, "Iterator does not point to a field");
    return mField;
}

FieldsTrait::Iterator& FieldsTrait::Iterator::operator++()
{
    CUBOS_ASSERT(mField, "Iterator does not point to a field");
    mField = mField->next();
    return *this;
}

FieldsTrait::FieldsTrait(FieldsTrait&& other) noexcept
    : mFirstField(other.mFirstField)
    , mLastField(other.mLastField)
{
    other.mFirstField = nullptr;
    other.mLastField = nullptr;
}

FieldsTrait::~FieldsTrait()
{
    while (mFirstField != nullptr)
    {
        auto* next = mFirstField->mNext;
        delete mFirstField;
        mFirstField = next;
    }
}

FieldsTrait&& FieldsTrait::withField(const Type& type, std::string name, AddressOf* addressOf) &&
{
    for (auto* field = mFirstField; field != nullptr; field = field->mNext)
    {
        CUBOS_ASSERT(field->mName != name, "Field '{}' already exists", name);
    }

    auto* field = new Field(type, std::move(name), addressOf);
    if (mFirstField != nullptr)
    {
        mLastField->mNext = field;
        mLastField = field;
    }
    else
    {
        mFirstField = field;
        mLastField = field;
    }

    return std::move(*this);
}

const FieldsTrait::Field* FieldsTrait::field(const std::string& name) const
{
    for (auto* field = mFirstField; field != nullptr; field = field->mNext)
    {
        if (field->mName == name)
        {
            return field;
        }
    }

    CUBOS_ERROR("No such field '{}'", name);
    return nullptr;
}

FieldsTrait::Iterator FieldsTrait::begin() const
{
    return Iterator{mFirstField};
}

FieldsTrait::Iterator FieldsTrait::end()
{
    return Iterator{nullptr};
}
