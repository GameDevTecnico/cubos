#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/tel/logging.hpp>

using namespace cubos::core::reflection;

CUBOS_REFLECT_IMPL(ConstructibleTrait)
{
    return Type::create("cubos::core::reflection::ConstructibleTrait");
}

ConstructibleTrait::CustomConstructor::CustomConstructor(
    std::vector<const Type*> argTypes, std::vector<std::string> argNames,
    memory::Function<void(void* instance, void** args) const> functor)
    : mArgTypes(memory::move(argTypes))
    , mArgNames(memory::move(argNames))
    , mFunctor(memory::move(functor))
{
    CUBOS_ASSERT(mFunctor != nullptr, "Functor must be non-null");
    CUBOS_ASSERT(mArgTypes.size() == mArgNames.size(), "Argument types and names must have the same size");
    for (const Type* argType : mArgTypes)
    {
        CUBOS_ASSERT(argType != nullptr, "Argument type must not be null");
    }
}

std::size_t ConstructibleTrait::CustomConstructor::argCount() const
{
    return mArgTypes.size();
}

const std::string& ConstructibleTrait::CustomConstructor::argName(std::size_t index) const
{
    CUBOS_ASSERT(index < mArgNames.size(), "Index out of bounds");
    return mArgNames[index];
}

const Type& ConstructibleTrait::CustomConstructor::argType(std::size_t index) const
{
    CUBOS_ASSERT(index < mArgTypes.size(), "Index out of bounds");
    return *mArgTypes[index];
}

void ConstructibleTrait::CustomConstructor::call(void* instance, void** args) const
{
    CUBOS_ASSERT(instance != nullptr, "Instance must not be null");
    CUBOS_ASSERT(args != nullptr, "Args must not be null");
    mFunctor(instance, args);
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

ConstructibleTrait&& ConstructibleTrait::withCustomConstructor(CustomConstructor customConstructor) &&
{
    this->addCustomConstructor(memory::move(customConstructor));
    return memory::move(*this);
}

void ConstructibleTrait::addCustomConstructor(CustomConstructor customConstructor)
{
    mCustomConstructors.emplace_back(memory::move(customConstructor));
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

std::size_t ConstructibleTrait::customConstructorCount() const
{
    return mCustomConstructors.size();
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

void ConstructibleTrait::customConstruct(std::size_t index, void* instance, void** args) const
{
    customConstructor(index).call(instance, args);
}

ConstructibleTrait::Destructor ConstructibleTrait::destructor() const
{
    return mDestructor;
}

ConstructibleTrait::DefaultConstructor ConstructibleTrait::defaultConstructor() const
{
    return mDefaultConstructor;
}

ConstructibleTrait::CopyConstructor ConstructibleTrait::copyConstructor() const
{
    return mCopyConstructor;
}

ConstructibleTrait::MoveConstructor ConstructibleTrait::moveConstructor() const
{
    return mMoveConstructor;
}

const ConstructibleTrait::CustomConstructor& ConstructibleTrait::customConstructor(std::size_t index) const
{
    CUBOS_ASSERT(index < mCustomConstructors.size(), "Index out of bounds");
    return mCustomConstructors[index];
}
