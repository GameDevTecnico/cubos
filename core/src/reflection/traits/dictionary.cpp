#include <cubos/core/reflection/traits/dictionary.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/tel/logging.hpp>

using cubos::core::reflection::DictionaryTrait;
using cubos::core::reflection::Type;

CUBOS_REFLECT_IMPL(DictionaryTrait)
{
    return Type::create("cubos::core::reflection::DictionaryTrait");
}

DictionaryTrait::DictionaryTrait(const Type& keyType, const Type& valueType, Length length, Begin begin, Find find,
                                 Advance advance, Stop stop, Key key, Value value)
    : mKeyType(keyType)
    , mValueType(valueType)
    , mLength(length)
    , mBegin(begin)
    , mFind(find)
    , mAdvance(advance)
    , mStop(stop)
    , mKey(key)
    , mValue(value)
{
}

void DictionaryTrait::setInsertDefault(InsertDefault insertDefault)
{
    CUBOS_ASSERT(!mInsertDefault, "Insert default already set");
    mInsertDefault = insertDefault;
}

void DictionaryTrait::setInsertCopy(InsertCopy insertCopy)
{
    CUBOS_ASSERT(!mInsertCopy, "Insert copy already set");
    mInsertCopy = insertCopy;
}

void DictionaryTrait::setInsertMove(InsertMove insertMove)
{
    CUBOS_ASSERT(!mInsertMove, "Insert move already set");
    mInsertMove = insertMove;
}

void DictionaryTrait::setErase(Erase erase)
{
    CUBOS_ASSERT(!mErase, "Erase already set");
    mErase = erase;
}

bool DictionaryTrait::hasInsertDefault() const
{
    return mInsertDefault != nullptr;
}

bool DictionaryTrait::hasInsertCopy() const
{
    return mInsertCopy != nullptr;
}

bool DictionaryTrait::hasInsertMove() const
{
    return mInsertMove != nullptr;
}

bool DictionaryTrait::hasErase() const
{
    return mErase != nullptr;
}

const Type& DictionaryTrait::keyType() const
{
    return mKeyType;
}

const Type& DictionaryTrait::valueType() const
{
    return mValueType;
}

DictionaryTrait::View DictionaryTrait::view(void* instance) const
{
    return View{*this, instance};
}

DictionaryTrait::ConstView DictionaryTrait::view(const void* instance) const
{
    return ConstView{*this, instance};
}

DictionaryTrait::View::View(const DictionaryTrait& trait, void* instance)
    : mTrait(trait)
    , mInstance(instance)
{
}

std::size_t DictionaryTrait::View::length() const
{
    return mTrait.mLength(mInstance);
}

DictionaryTrait::View::Iterator DictionaryTrait::View::begin() const
{
    return Iterator{*this, mTrait.mBegin(reinterpret_cast<uintptr_t>(mInstance), true)};
}

DictionaryTrait::View::Iterator DictionaryTrait::View::end() const
{
    return Iterator{*this, nullptr};
}

DictionaryTrait::View::Iterator DictionaryTrait::View::find(const void* key) const
{
    return Iterator{*this, mTrait.mFind(reinterpret_cast<uintptr_t>(mInstance), key, true)};
}

void DictionaryTrait::View::insertDefault(const void* key) const
{
    CUBOS_ASSERT(mTrait.hasInsertDefault(), "Insert default not supported");
    mTrait.mInsertDefault(mInstance, key);
}

void DictionaryTrait::View::insertCopy(const void* key, const void* value) const
{
    CUBOS_ASSERT(mTrait.hasInsertCopy(), "Insert copy not supported");
    mTrait.mInsertCopy(mInstance, key, value);
}

void DictionaryTrait::View::insertMove(const void* key, void* value) const
{
    CUBOS_ASSERT(mTrait.hasInsertMove(), "Insert move not supported");
    mTrait.mInsertMove(mInstance, key, value);
}

void DictionaryTrait::View::erase(Iterator& iterator) const
{
    CUBOS_ASSERT(mTrait.hasErase(), "Erase not supported");
    mTrait.mErase(mInstance, iterator.mInner);
    mTrait.mStop(iterator.mInner, true);
    iterator.mInner = nullptr;
}

void DictionaryTrait::View::erase(Iterator&& iterator) const
{
    this->erase(iterator);
}

void DictionaryTrait::View::clear() const
{
    // This really inefficient, but if it ever becomes a problem its easy to improve, we could just
    // add yet another function pointer to the trait, which each dictionary type sets.
    while (this->length() > 0)
    {
        this->erase(this->begin());
    }
}

DictionaryTrait::ConstView::ConstView(const DictionaryTrait& trait, const void* instance)
    : mTrait(trait)
    , mInstance(instance)
{
}

std::size_t DictionaryTrait::ConstView::length() const
{
    return mTrait.mLength(mInstance);
}

DictionaryTrait::ConstView::Iterator DictionaryTrait::ConstView::begin() const
{
    return Iterator{*this, mTrait.mBegin(reinterpret_cast<uintptr_t>(mInstance), false)};
}

DictionaryTrait::ConstView::Iterator DictionaryTrait::ConstView::end() const
{
    return Iterator{*this, nullptr};
}

DictionaryTrait::ConstView::Iterator DictionaryTrait::ConstView::find(const void* key) const
{
    return Iterator{*this, mTrait.mFind(reinterpret_cast<uintptr_t>(mInstance), key, false)};
}

DictionaryTrait::View::Iterator::~Iterator()
{
    if (mInner != nullptr)
    {
        mView.mTrait.mStop(mInner, true);
    }
}

DictionaryTrait::View::Iterator::Iterator(const View& view, void* inner)
    : mView(view)
    , mInner(inner)
{
}

DictionaryTrait::View::Iterator::Iterator(const Iterator& other)
    : mView(other.mView)
{
    if (other.mInner != nullptr)
    {
        mInner = mView.mTrait.mFind(reinterpret_cast<uintptr_t>(mView.mInstance), other->key, true);
    }
}

DictionaryTrait::View::Iterator::Iterator(Iterator&& other) noexcept
    : mView(other.mView)
    , mInner(other.mInner)
{
    other.mInner = nullptr;
}

DictionaryTrait::View::Iterator& DictionaryTrait::View::Iterator::operator=(const Iterator& other)
{
    if (mInner != nullptr)
    {
        mView.mTrait.mStop(mInner, true);
        mInner = nullptr;
    }

    if (other.mInner != nullptr)
    {
        mInner = mView.mTrait.mFind(reinterpret_cast<uintptr_t>(mView.mInstance), other->key, true);
    }

    return *this;
}

bool DictionaryTrait::View::Iterator::operator==(const Iterator& other) const
{
    return mInner == other.mInner || (mInner != nullptr && other.mInner != nullptr && (*this)->value == other->value);
}

bool DictionaryTrait::View::Iterator::operator!=(const Iterator& other) const
{
    return !(*this == other);
}

const DictionaryTrait::View::Iterator::Entry& DictionaryTrait::View::Iterator::operator*() const
{
    CUBOS_ASSERT(mInner != nullptr, "Iterator out of bounds");
    mEntry.key = mView.mTrait.mKey(mInner, true);
    mEntry.value = reinterpret_cast<void*>(mView.mTrait.mValue(mInner, true));
    return mEntry;
}

const DictionaryTrait::View::Iterator::Entry* DictionaryTrait::View::Iterator::operator->() const
{
    return &this->operator*();
}

DictionaryTrait::View::Iterator& DictionaryTrait::View::Iterator::operator++()
{
    CUBOS_ASSERT(mInner != nullptr, "Iterator out of bounds");

    if (!mView.mTrait.mAdvance(reinterpret_cast<uintptr_t>(mView.mInstance), mInner, true))
    {
        mView.mTrait.mStop(mInner, true);
        mInner = nullptr;
    }

    return *this;
}

DictionaryTrait::ConstView::Iterator::~Iterator()
{
    if (mInner != nullptr)
    {
        mView.mTrait.mStop(mInner, false);
    }
}

DictionaryTrait::ConstView::Iterator::Iterator(const ConstView& view, void* inner)
    : mView(view)
    , mInner(inner)
{
}

DictionaryTrait::ConstView::Iterator::Iterator(const Iterator& other)
    : mView(other.mView)
{
    if (other.mInner != nullptr)
    {
        mInner = mView.mTrait.mFind(reinterpret_cast<uintptr_t>(mView.mInstance), other->key, false);
    }
}

DictionaryTrait::ConstView::Iterator::Iterator(Iterator&& other) noexcept
    : mView(other.mView)
    , mInner(other.mInner)
{
    other.mInner = nullptr;
}

DictionaryTrait::ConstView::Iterator& DictionaryTrait::ConstView::Iterator::operator=(const Iterator& other)
{
    if (mInner != nullptr)
    {
        mView.mTrait.mStop(mInner, false);
        mInner = nullptr;
    }

    if (other.mInner != nullptr)
    {
        mInner = mView.mTrait.mFind(reinterpret_cast<uintptr_t>(mView.mInstance), other->key, false);
    }

    return *this;
}

bool DictionaryTrait::ConstView::Iterator::operator==(const Iterator& other) const
{
    return mInner == other.mInner || (mInner != nullptr && other.mInner != nullptr && (*this)->value == other->value);
}

bool DictionaryTrait::ConstView::Iterator::operator!=(const Iterator& other) const
{
    return !(*this == other);
}

const DictionaryTrait::ConstView::Iterator::Entry& DictionaryTrait::ConstView::Iterator::operator*() const
{
    CUBOS_ASSERT(mInner != nullptr, "Iterator out of bounds");
    mEntry.key = mView.mTrait.mKey(mInner, false);
    mEntry.value = reinterpret_cast<void*>(mView.mTrait.mValue(mInner, false));
    return mEntry;
}

const DictionaryTrait::ConstView::Iterator::Entry* DictionaryTrait::ConstView::Iterator::operator->() const
{
    return &this->operator*();
}

DictionaryTrait::ConstView::Iterator& DictionaryTrait::ConstView::Iterator::operator++()
{
    CUBOS_ASSERT(mInner != nullptr, "Iterator out of bounds");

    if (!mView.mTrait.mAdvance(reinterpret_cast<uintptr_t>(mView.mInstance), mInner, true))
    {
        mView.mTrait.mStop(mInner, true);
        mInner = nullptr;
    }

    return *this;
}
