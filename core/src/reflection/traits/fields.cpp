#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/tel/logging.hpp>

using namespace cubos::core::reflection;

CUBOS_REFLECT_IMPL(FieldsTrait)
{
    return Type::create("cubos::core::reflection::FieldsTrait");
}

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

void FieldsTrait::addField(const Type& type, std::string name, AddressOf* addressOf)
{
    for (auto* field = mFirstField; field != nullptr; field = field->mNext)
    {
        CUBOS_ASSERT(field->mName != name, "Field {} already exists", name);
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
}

FieldsTrait&& FieldsTrait::withField(const Type& type, std::string name, AddressOf* addressOf) &&
{
    this->addField(type, std::move(name), addressOf);
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

    CUBOS_DEBUG("No such field {}", name);
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

std::size_t FieldsTrait::size() const
{
    std::size_t i = 0;
    for (const auto& field : *this)
    {
        (void)field;
        i += 1;
    }
    return i;
}

FieldsTrait::View FieldsTrait::view(void* instance) const
{
    return View{*this, instance};
}

FieldsTrait::ConstView FieldsTrait::view(const void* instance) const
{
    return ConstView{*this, instance};
}

FieldsTrait::View::View(const FieldsTrait& trait, void* instance)
    : mTrait{trait}
    , mInstance{instance}
{
}

void* FieldsTrait::View::get(const Field& field) const
{
    return field.pointerTo(mInstance);
}

void* FieldsTrait::View::get(const std::string& name) const
{
    const auto* field = mTrait.field(name);

    if (field == nullptr)
    {
        return nullptr;
    }

    return this->get(*field);
}

FieldsTrait::View::Iterator FieldsTrait::View::begin() const
{
    return Iterator{*this, mTrait.mFirstField};
}

FieldsTrait::View::Iterator FieldsTrait::View::end() const
{
    return Iterator{*this, nullptr};
}

FieldsTrait::ConstView::ConstView(const FieldsTrait& trait, const void* instance)
    : mTrait{trait}
    , mInstance{instance}
{
}

const void* FieldsTrait::ConstView::get(const Field& field) const
{
    return field.pointerTo(mInstance);
}

const void* FieldsTrait::ConstView::get(const std::string& name) const
{
    const auto* field = mTrait.field(name);

    if (field == nullptr)
    {
        return nullptr;
    }

    return this->get(*field);
}

FieldsTrait::ConstView::Iterator FieldsTrait::ConstView::begin() const
{
    return Iterator{*this, mTrait.mFirstField};
}

FieldsTrait::ConstView::Iterator FieldsTrait::ConstView::end() const
{
    return Iterator{*this, nullptr};
}

FieldsTrait::View::Iterator::Iterator(const View& view, const Field* field)
    : mInstance{view.mInstance}
    , mField{field}
{
}

bool FieldsTrait::View::Iterator::operator==(const Iterator& other) const
{
    return mInstance == other.mInstance && mField == other.mField;
}

bool FieldsTrait::View::Iterator::operator!=(const Iterator& other) const
{
    return !(*this == other);
}

const FieldsTrait::View::Iterator::Output& FieldsTrait::View::Iterator::operator*() const
{
    CUBOS_ASSERT(mField != nullptr, "Iterator out of bounds");
    mOutput.field = mField;
    mOutput.value = mField->pointerTo(mInstance);
    return mOutput;
}

const FieldsTrait::View::Iterator::Output* FieldsTrait::View::Iterator::operator->() const
{
    return &this->operator*();
}

FieldsTrait::View::Iterator& FieldsTrait::View::Iterator::operator++()
{
    CUBOS_ASSERT(mField != nullptr, "Iterator out of bounds");
    mField = mField->next();
    return *this;
}

FieldsTrait::ConstView::Iterator::Iterator(const ConstView& view, const Field* field)
    : mInstance{view.mInstance}
    , mField{field}
{
}

bool FieldsTrait::ConstView::Iterator::operator==(const Iterator& other) const
{
    return mInstance == other.mInstance && mField == other.mField;
}

bool FieldsTrait::ConstView::Iterator::operator!=(const Iterator& other) const
{
    return !(*this == other);
}

const FieldsTrait::ConstView::Iterator::Output& FieldsTrait::ConstView::Iterator::operator*() const
{
    CUBOS_ASSERT(mField != nullptr, "Iterator out of bounds");
    mOutput.field = mField;
    mOutput.value = mField->pointerTo(mInstance);
    return mOutput;
}

const FieldsTrait::ConstView::Iterator::Output* FieldsTrait::ConstView::Iterator::operator->() const
{
    return &this->operator*();
}

FieldsTrait::ConstView::Iterator& FieldsTrait::ConstView::Iterator::operator++()
{
    CUBOS_ASSERT(mField != nullptr, "Iterator out of bounds");
    mField = mField->next();
    return *this;
}
