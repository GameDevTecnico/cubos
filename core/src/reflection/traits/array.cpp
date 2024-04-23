#include <cubos/core/log.hpp>
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::ArrayTrait;
using cubos::core::reflection::Type;

CUBOS_REFLECT_IMPL(ArrayTrait)
{
    return Type::create("cubos::core::ecs::ArrayTrait");
}

ArrayTrait::ArrayTrait(const Type& elementType, Length length, AddressOf addressOf)
    : mElementType(elementType)
    , mLength(length)
    , mAddressOf(addressOf)
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

bool ArrayTrait::hasResize() const
{
    return this->hasErase() && this->hasInsertDefault();
}

const Type& ArrayTrait::elementType() const
{
    return mElementType;
}

ArrayTrait::View ArrayTrait::view(void* instance) const
{
    return View{*this, instance};
}

ArrayTrait::ConstView ArrayTrait::view(const void* instance) const
{
    return ConstView{*this, instance};
}

ArrayTrait::View::View(const ArrayTrait& trait, void* instance)
    : mTrait(trait)
    , mInstance(instance)
{
    CUBOS_ASSERT(mInstance != nullptr, "Instance must not be null");
}

std::size_t ArrayTrait::View::length() const
{
    return mTrait.mLength(mInstance);
}

void* ArrayTrait::View::get(std::size_t index) const
{
    return reinterpret_cast<void*>(mTrait.mAddressOf(mInstance, index));
}

void ArrayTrait::View::insertDefault(std::size_t index) const
{
    CUBOS_ASSERT(mTrait.hasInsertDefault(), "Insert default not supported");
    mTrait.mInsertDefault(mInstance, index);
}

void ArrayTrait::View::insertCopy(std::size_t index, const void* value) const
{
    CUBOS_ASSERT(mTrait.hasInsertCopy(), "Insert copy not supported");
    mTrait.mInsertCopy(mInstance, index, value);
}

void ArrayTrait::View::insertMove(std::size_t index, void* value) const
{
    CUBOS_ASSERT(mTrait.hasInsertMove(), "Insert move not supported");
    mTrait.mInsertMove(mInstance, index, value);
}

void ArrayTrait::View::erase(std::size_t index) const
{
    CUBOS_ASSERT(mTrait.hasErase(), "Erase not supported");
    mTrait.mErase(mInstance, index);
}

void ArrayTrait::View::clear() const
{
    CUBOS_ASSERT(mTrait.hasErase(), "Clear not supported");

    // This really inefficient, but if it ever becomes a problem its easy to improve, we could just
    // add yet another function pointer to the trait, which each array type sets.
    while (this->length() > 0)
    {
        this->erase(this->length() - 1);
    }
}

void ArrayTrait::View::resize(std::size_t size) const
{
    CUBOS_ASSERT(mTrait.hasResize(), "Resize not supported");

    // This really inefficient, but if it ever becomes a problem its easy to improve, we could just
    // add yet another function pointer to the trait, which each array type sets.
    while (this->length() > size)
    {
        this->erase(this->length() - 1);
    }

    while (this->length() < size)
    {
        this->insertDefault(this->length());
    }
}

ArrayTrait::View::Iterator ArrayTrait::View::begin() const
{
    return Iterator{*this, 0};
}

ArrayTrait::View::Iterator ArrayTrait::View::end() const
{
    return Iterator{*this, this->length()};
}

ArrayTrait::ConstView::ConstView(const ArrayTrait& trait, const void* instance)
    : mTrait(trait)
    , mInstance(instance)
{
    CUBOS_ASSERT(mInstance != nullptr, "Instance must not be null");
}

std::size_t ArrayTrait::ConstView::length() const
{
    return mTrait.mLength(mInstance);
}

const void* ArrayTrait::ConstView::get(std::size_t index) const
{
    return reinterpret_cast<const void*>(mTrait.mAddressOf(mInstance, index));
}

ArrayTrait::ConstView::Iterator ArrayTrait::ConstView::begin() const
{
    return Iterator{*this, 0};
}

ArrayTrait::ConstView::Iterator ArrayTrait::ConstView::end() const
{
    return Iterator{*this, this->length()};
}

ArrayTrait::View::Iterator::Iterator(const View& view, std::size_t index)
    : mView(&view)
    , mIndex(index)
{
}

void* ArrayTrait::View::Iterator::operator*() const
{
    CUBOS_ASSERT(mIndex < mView->length(), "Iterator out of bounds");
    return mView->get(mIndex);
}

void* ArrayTrait::View::Iterator::operator->() const
{
    return this->operator*();
}

ArrayTrait::View::Iterator& ArrayTrait::View::Iterator::operator++()
{
    CUBOS_ASSERT(mIndex < mView->length(), "Iterator out of bounds");
    ++mIndex;
    return *this;
}

ArrayTrait::ConstView::Iterator::Iterator(const ConstView& view, std::size_t index)
    : mView(&view)
    , mIndex(index)
{
}

const void* ArrayTrait::ConstView::Iterator::operator*() const
{
    CUBOS_ASSERT(mIndex < mView->length(), "Iterator out of bounds");
    return mView->get(mIndex);
}

const void* ArrayTrait::ConstView::Iterator::operator->() const
{
    return this->operator*();
}

ArrayTrait::ConstView::Iterator& ArrayTrait::ConstView::Iterator::operator++()
{
    CUBOS_ASSERT(mIndex < mView->length(), "Iterator out of bounds");
    ++mIndex;
    return *this;
}
