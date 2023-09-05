#include <cubos/core/log.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

using namespace cubos::core::reflection;

ConstructibleTrait::ConstructibleTrait(std::size_t size, std::size_t alignment, void (*destructor)(void*))
    : mSize(size)
    , mAlignment(alignment)
    , mDestructor(destructor)
    , mDefaultConstructor(nullptr)
    , mCopyConstructor(nullptr)
    , mMoveConstructor(nullptr)
{
    CUBOS_ASSERT(mAlignment > 0, "Alignment must be positive");
    CUBOS_ASSERT((mAlignment & (mAlignment - 1)) == 0, "Alignment must be a power of two");
    CUBOS_ASSERT(mDestructor, "Destructor must be non-null");
}

ConstructibleTrait& ConstructibleTrait::withDefaultConstructor(DefaultConstructor defaultConstructor)
{
    CUBOS_ASSERT(!mDefaultConstructor, "Default constructor already set");
    mDefaultConstructor = defaultConstructor;
    return *this;
}

ConstructibleTrait& ConstructibleTrait::withCopyConstructor(CopyConstructor copyConstructor)
{
    CUBOS_ASSERT(!mCopyConstructor, "Copy constructor already set");
    mCopyConstructor = copyConstructor;
    return *this;
}

ConstructibleTrait& ConstructibleTrait::withMoveConstructor(MoveConstructor moveConstructor)
{
    CUBOS_ASSERT(!mMoveConstructor, "Move constructor already set");
    mMoveConstructor = moveConstructor;
    return *this;
}

std::size_t ConstructibleTrait::size() const
{
    return mSize;
}

std::size_t ConstructibleTrait::alignment() const
{
    return mAlignment;
}

void ConstructibleTrait::destruct(void* instance) const
{
    mDestructor(instance);
}

bool ConstructibleTrait::defaultConstruct(void* instance) const
{
    if (mDefaultConstructor)
    {
        mDefaultConstructor(instance);
        return true;
    }

    return false;
}

bool ConstructibleTrait::copyConstruct(void* instance, const void* other) const
{
    if (mCopyConstructor)
    {
        mCopyConstructor(instance, other);
        return true;
    }

    return false;
}

bool ConstructibleTrait::moveConstruct(void* instance, void* other) const
{
    if (mMoveConstructor)
    {
        mMoveConstructor(instance, other);
        return true;
    }

    return false;
}
