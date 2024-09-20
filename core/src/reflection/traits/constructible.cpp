#include <cubos/core/log.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

using namespace cubos::core::reflection;

CUBOS_REFLECT_IMPL(ConstructibleTrait)
{
    return Type::create("cubos::core::ecs::ConstructibleTrait");
}

ConstructibleTrait::ConstructibleTrait(std::size_t size, std::size_t alignment, Destructor destructor)
    : mSize(size)
    , mAlignment(alignment)
    , mDestructor(memory::move(destructor))
{
    CUBOS_ASSERT(mAlignment > 0, "Alignment must be positive");
    CUBOS_ASSERT((mAlignment & (mAlignment - 1)) == 0, "Alignment must be a power of two");
    CUBOS_ASSERT(mDestructor != nullptr, "Destructor must be non-null");
}

ConstructibleTrait&& ConstructibleTrait::withDefaultConstructor(DefaultConstructor defaultConstructor) &&
{
    CUBOS_ASSERT(mDefaultConstructor == nullptr, "Default constructor already set");
    mDefaultConstructor = memory::move(defaultConstructor);
    return memory::move(*this);
}

ConstructibleTrait&& ConstructibleTrait::withCopyConstructor(CopyConstructor copyConstructor) &&
{
    CUBOS_ASSERT(mCopyConstructor == nullptr, "Copy constructor already set");
    mCopyConstructor = memory::move(copyConstructor);
    return memory::move(*this);
}

ConstructibleTrait&& ConstructibleTrait::withMoveConstructor(MoveConstructor moveConstructor) &&
{
    CUBOS_ASSERT(mMoveConstructor == nullptr, "Move constructor already set");
    mMoveConstructor = memory::move(moveConstructor);
    return memory::move(*this);
}

std::size_t ConstructibleTrait::size() const
{
    return mSize;
}

std::size_t ConstructibleTrait::alignment() const
{
    return mAlignment;
}

bool ConstructibleTrait::hasDefaultConstruct() const
{
    return mDefaultConstructor != nullptr;
}

bool ConstructibleTrait::hasCopyConstruct() const
{
    return mCopyConstructor != nullptr;
}

bool ConstructibleTrait::hasMoveConstruct() const
{
    return mMoveConstructor != nullptr;
}

void ConstructibleTrait::destruct(void* instance) const
{
    mDestructor(instance);
}

void ConstructibleTrait::defaultConstruct(void* instance) const
{
    CUBOS_ASSERT(this->hasDefaultConstruct());
    mDefaultConstructor(instance);
}

void ConstructibleTrait::copyConstruct(void* instance, const void* other) const
{
    CUBOS_ASSERT(this->hasCopyConstruct());
    mCopyConstructor(instance, other);
}

void ConstructibleTrait::moveConstruct(void* instance, void* other) const
{
    CUBOS_ASSERT(this->hasMoveConstruct());
    mMoveConstructor(instance, other);
}
