#include <cubos/core/log.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::Type;

Type::Builder::Builder(Type& type)
    : mType(type)
{
}

Type::Builder& Type::Builder::defaultConstructor(DefaultConstructor constructor)
{
    mType.mDefaultConstructor = constructor;
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

void* Type::defaultConstruct() const
{
    CUBOS_ASSERT(mDefaultConstructor, "No default constructor set");
    return mDefaultConstructor();
}

void Type::destroy(void* object) const
{
    CUBOS_ASSERT(mDestructor, "No destructor set");
    mDestructor(object);
}
