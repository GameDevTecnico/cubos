#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/mask.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/tel/logging.hpp>

using cubos::core::reflection::MaskTrait;

CUBOS_REFLECT_IMPL(MaskTrait)
{
    return Type::create("cubos::core::reflection::MaskTrait");
}

MaskTrait::~MaskTrait()
{
    while (mFirstBit != nullptr)
    {
        auto* next = mFirstBit->mNext;
        delete mFirstBit;
        mFirstBit = next;
    }
}

MaskTrait::MaskTrait(MaskTrait&& other) noexcept
    : mFirstBit(other.mFirstBit)
    , mLastBit(other.mLastBit)
{
    other.mFirstBit = nullptr;
    other.mLastBit = nullptr;
}

void MaskTrait::addBit(std::string name, Tester tester, Setter setter, Clearer clearer)
{
    for (auto* bit = mFirstBit; bit != nullptr; bit = bit->mNext)
    {
        CUBOS_ASSERT(bit->mName != name, "Mask bit with name {} already exists", name);
    }

    auto* bit = new Bit(std::move(name), std::move(tester), std::move(setter), std::move(clearer));
    if (mFirstBit != nullptr)
    {
        mLastBit->mNext = bit;
        mLastBit = bit;
    }
    else
    {
        mFirstBit = bit;
        mLastBit = bit;
    }
}

MaskTrait&& MaskTrait::withBit(std::string name, Tester tester, Setter setter, Clearer clearer) &&
{
    this->addBit(std::move(name), std::move(tester), std::move(setter), std::move(clearer));
    return std::move(*this);
}

bool MaskTrait::contains(const std::string& name) const
{
    for (auto* bit = mFirstBit; bit != nullptr; bit = bit->mNext)
    {
        if (bit->mName == name)
        {
            return true;
        }
    }
    return false;
}

const MaskTrait::Bit& MaskTrait::at(const std::string& name) const
{
    for (const auto* bit = mFirstBit; bit != nullptr; bit = bit->mNext)
    {
        if (bit->mName == name)
        {
            return *bit;
        }
    }
    CUBOS_FAIL("Bit with name {} does not exist", name);
}

auto MaskTrait::begin() const -> Iterator
{
    return Iterator{mFirstBit};
}

auto MaskTrait::end() -> Iterator
{
    return Iterator{nullptr};
}

std::size_t MaskTrait::size() const
{
    std::size_t i = 0;
    for (const auto& bit : *this)
    {
        (void)bit;
        i += 1;
    }
    return i;
}

auto MaskTrait::view(void* instance) const -> View
{
    return View{*this, instance};
}

auto MaskTrait::view(const void* instance) const -> ConstView
{
    return ConstView{*this, instance};
}

MaskTrait::Bit::Bit(std::string name, Tester tester, Setter setter, Clearer clearer)
    : mName{std::move(name)}
    , mTester{std::move(tester)}
    , mSetter{std::move(setter)}
    , mClearer{std::move(clearer)}
{
}

bool MaskTrait::Bit::test(const void* value) const
{
    return mTester(value);
}

void MaskTrait::Bit::set(void* value) const
{
    mSetter(value);
}

void MaskTrait::Bit::clear(void* value) const
{
    mClearer(value);
}

const std::string& MaskTrait::Bit::name() const
{
    return mName;
}

const MaskTrait::Bit* MaskTrait::Bit::next() const
{
    return mNext;
}

MaskTrait::Iterator::Iterator(const Bit* bit)
    : mBit{bit}
{
}

const MaskTrait::Bit& MaskTrait::Iterator::operator*() const
{
    CUBOS_ASSERT(mBit, "Iterator does not point to a bit");
    return *mBit;
}

const MaskTrait::Bit* MaskTrait::Iterator::operator->() const
{
    CUBOS_ASSERT(mBit, "Iterator does not point to a bit");
    return mBit;
}

MaskTrait::Iterator& MaskTrait::Iterator::operator++()
{
    CUBOS_ASSERT(mBit, "Iterator does not point to a bit");
    mBit = mBit->next();
    return *this;
}

MaskTrait::SetIterator::SetIterator(const Bit* bit, const void* instance)
    : mBit{bit}
    , mInstance{instance}
{
    // Advance to the first bit that is set.
    while (mBit != nullptr && !mBit->test(mInstance))
    {
        mBit = mBit->next();
    }
}

const MaskTrait::Bit& MaskTrait::SetIterator::operator*() const
{
    CUBOS_ASSERT(mBit, "Iterator does not point to a bit");
    return *mBit;
}

const MaskTrait::Bit* MaskTrait::SetIterator::operator->() const
{
    CUBOS_ASSERT(mBit, "Iterator does not point to a bit");
    return mBit;
}

MaskTrait::SetIterator& MaskTrait::SetIterator::operator++()
{
    CUBOS_ASSERT(mBit, "Iterator does not point to a bit");

    // Advance to the next bit that is also set.
    mBit = mBit->next();
    while (mBit != nullptr && !mBit->test(mInstance))
    {
        mBit = mBit->next();
    }

    return *this;
}

MaskTrait::View::View(const MaskTrait& trait, void* instance)
    : mTrait{trait}
    , mInstance{instance}
{
}

bool MaskTrait::View::test(const Bit& bit) const
{
    return bit.test(mInstance);
}

bool MaskTrait::View::test(const std::string& name) const
{
    return mTrait.at(name).test(mInstance);
}

void MaskTrait::View::set(const Bit& bit) const
{
    bit.set(mInstance);
}

void MaskTrait::View::set(const std::string& name) const
{
    mTrait.at(name).set(mInstance);
}

void MaskTrait::View::clear(const Bit& bit) const
{
    bit.clear(mInstance);
}

void MaskTrait::View::clear(const std::string& name) const
{
    mTrait.at(name).clear(mInstance);
}

MaskTrait::SetIterator MaskTrait::View::begin() const
{
    return {mTrait.mFirstBit, mInstance};
}

MaskTrait::SetIterator MaskTrait::View::end() const
{
    return {nullptr, mInstance};
}

MaskTrait::ConstView::ConstView(const MaskTrait& trait, const void* instance)
    : mTrait{trait}
    , mInstance{instance}
{
}

bool MaskTrait::ConstView::test(const Bit& bit) const
{
    return bit.test(mInstance);
}

bool MaskTrait::ConstView::test(const std::string& name) const
{
    return mTrait.at(name).test(mInstance);
}

MaskTrait::SetIterator MaskTrait::ConstView::begin() const
{
    return {mTrait.mFirstBit, mInstance};
}

MaskTrait::SetIterator MaskTrait::ConstView::end() const
{
    return {nullptr, mInstance};
}
