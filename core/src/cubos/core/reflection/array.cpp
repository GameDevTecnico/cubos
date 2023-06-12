#include <cubos/core/reflection/array.hpp>
#include <cubos/core/reflection/visitor.hpp>

using cubos::core::reflection::ArrayType;
using cubos::core::reflection::Type;

ArrayType::Builder::Builder(ArrayType& type)
    : Type::Builder(type)
    , mType(type)
{
}

ArrayType::Builder& ArrayType::Builder::length(Length length)
{
    mType.mLength = length;
    return *this;
}

ArrayType::Builder& ArrayType::Builder::element(Element element)
{
    mType.mElement = element;
    return *this;
}

ArrayType::Builder& ArrayType::Builder::resize(Resize resize)
{
    mType.mResize = resize;
    return *this;
}

ArrayType::Builder ArrayType::build(std::string name, const Type& elementType)
{
    return Builder(*new ArrayType{std::move(name), elementType});
}

const Type& ArrayType::elementType() const
{
    return mElementType;
}

ArrayType::Length ArrayType::length() const
{
    return mLength;
}

ArrayType::Element ArrayType::element() const
{
    return mElement;
}

ArrayType::Resize ArrayType::resize() const
{
    return mResize;
}

std::size_t ArrayType::length(const void* array) const
{
    CUBOS_ASSERT(mLength, "No length getter function set");
    return mLength(array);
}

const void* ArrayType::element(const void* array, std::size_t index) const
{
    CUBOS_ASSERT(mElement, "No element getter function set");
    return reinterpret_cast<const char*>(mElement(array, index));
}

void* ArrayType::element(void* array, std::size_t index) const
{
    CUBOS_ASSERT(mElement, "No element getter function set");
    return reinterpret_cast<char*>(mElement(array, index));
}

void ArrayType::resize(void* array, std::size_t length) const
{
    CUBOS_ASSERT(mResize, "No resize function set");
    mResize(array, length);
}

void ArrayType::accept(TypeVisitor& visitor) const
{
    visitor.visit(*this);
}

ArrayType::ArrayType(std::string name, const Type& elementType)
    : Type(std::move(name))
    , mElementType(elementType)
    , mLength(nullptr)
    , mElement(nullptr)
    , mResize(nullptr)
{
}
