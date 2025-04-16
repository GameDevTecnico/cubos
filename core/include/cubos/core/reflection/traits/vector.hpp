/// @file
/// @brief Class @ref cubos::core::reflection::VectorTrait.
/// @ingroup core-reflection

#pragma once

#include <string>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::reflection
{
    /// @brief Trait for types which represent math vectors using fixed-size contiguous arrays of scalars.
    ///
    /// Assumes the vector is represented as a contiguous array of scalars, with the first element at
    /// offset 0, the second at offset `scalarType.size()`, and so on. The scalar size must be a multiple of its
    /// alignment.
    ///
    /// @see See @ref examples-core-reflection-traits-vector for an example of using this trait.
    /// @ingroup core-reflection
    class CUBOS_CORE_API VectorTrait
    {
    public:
        CUBOS_REFLECT;

        /// @brief Constructs.
        /// @warning Aborts if the scalar type isn't @ref ConstructibleTrait "constructible".
        /// @param scalarType Type of the scalar elements of the vector.
        /// @param dimensions Number of dimensions.
        VectorTrait(const Type& scalarType, std::size_t dimensions);

        /// @brief Returns the type of the scalar elements of the vector.
        /// @return Type of the scalar elements of the vector.
        const Type& scalarType() const;

        /// @brief Returns the number of dimensions of the vector.
        /// @return Number of dimensions of the vector.
        std::size_t dimensions() const;

        /// @brief Accesses the scalar element at the given index.
        /// @param value Pointer to the vector instance.
        /// @param index Index of the scalar element to access.
        /// @return Pointer to the scalar element at the given index.
        void* get(void* value, std::size_t index) const;

        /// @brief Accesses the scalar element at the given index.
        /// @param value Pointer to the vector instance.
        /// @param index Index of the scalar element to access.
        /// @return Pointer to the scalar element at the given index.
        const void* get(const void* value, std::size_t index) const;

    private:
        const Type& mScalarType;   ///< Type of the scalar elements of the vector.
        std::size_t mDimensions;   ///< Number of dimensions of the vector.
        std::size_t mScalarStride; ///< Distance in bytes between two consecutive scalar elements.
    };
} // namespace cubos::core::reflection
