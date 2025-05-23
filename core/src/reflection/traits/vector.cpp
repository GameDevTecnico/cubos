#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/vector.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/tel/logging.hpp>

using cubos::core::reflection::Type;
using cubos::core::reflection::VectorTrait;

CUBOS_REFLECT_IMPL(VectorTrait)
{
    return Type::create("cubos::core::reflection::VectorTrait");
}

VectorTrait::VectorTrait(const Type& scalarType, std::size_t dimensions)
    : mScalarType(scalarType)
    , mDimensions(dimensions)
{
    CUBOS_ASSERT(scalarType.has<ConstructibleTrait>(), "Vector scalar types must be constructible");
    const auto& trait = scalarType.get<ConstructibleTrait>();

    // Assume the offset in bytes between elements is the same as the size of the scalar type.
    mScalarStride = trait.size();
    CUBOS_ASSERT(mScalarStride % trait.alignment() == 0, "Scalar type size must be a multiple of its alignment");
}

const Type& VectorTrait::scalarType() const
{
    return mScalarType;
}

std::size_t VectorTrait::dimensions() const
{
    return mDimensions;
}

void* VectorTrait::get(void* value, std::size_t index) const
{
    CUBOS_ASSERT(index < mDimensions, "Index out of bounds");
    return static_cast<char*>(value) + index * mScalarStride;
}

const void* VectorTrait::get(const void* value, std::size_t index) const
{
    return this->get(const_cast<void*>(value), index);
}
