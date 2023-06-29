#include <cubos/core/log.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::Type;

Type::Builder::Builder(Type& type)
    : mType(type)
{
}

Type::Builder& Type::Builder::layout(std::size_t size, std::size_t alignment)
{
    mType.mSize = size;
    mType.mAlignment = alignment;
    return *this;
}

Type::Builder& Type::Builder::defaultConstructor(DefaultConstructor constructor)
{
    CUBOS_ASSERT(mType.mAlignment > 0, "Type alignment must be greater than 0, did you forget to call layout()?");
    mType.mDefaultConstructor = constructor;
    return *this;
}

Type::Builder& Type::Builder::moveConstructor(MoveConstructor constructor)
{
    CUBOS_ASSERT(mType.mAlignment > 0, "Type alignment must be greater than 0, did you forget to call layout()?");
    mType.mMoveConstructor = constructor;
    return *this;
}

Type::Builder& Type::Builder::destructor(Destructor destructor)
{
    mType.mDestructor = destructor;
    return *this;
}

Type* Type::Builder::get()
{
    return &mType;
}

Type::Type(std::string name)
    : mName(std::move(name))
    , mSize(0)
    , mAlignment(0)
    , mDefaultConstructor(nullptr)
    , mDestructor(nullptr)
{
    mShortName = mName.substr(0, mName.find_first_of('<'));
}

Type::DefaultConstructor Type::defaultConstructor() const
{
    return mDefaultConstructor;
}

Type::Destructor Type::destructor() const
{
    return mDestructor;
}

const std::string& Type::name() const
{
    return mName;
}

const std::string& Type::shortName() const
{
    return mShortName;
}

std::size_t Type::size() const
{
    return mSize;
}

std::size_t Type::alignment() const
{
    return mAlignment;
}

void Type::defaultConstruct(void* ptr) const
{
    CUBOS_ASSERT(mDefaultConstructor, "No default constructor set");
    mDefaultConstructor(ptr);
}

void Type::moveConstruct(void* ptr, void* source) const
{
    CUBOS_ASSERT(mMoveConstructor, "No move constructor set");
    mMoveConstructor(ptr, source);
}

void Type::destroy(void* object) const
{
    CUBOS_ASSERT(mDestructor, "No destructor set");
    mDestructor(object);
}
