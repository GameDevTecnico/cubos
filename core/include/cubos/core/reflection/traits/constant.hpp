/// @file
/// @brief Class @ref cubos::core::reflection::ConstantTrait.
/// @ingroup core-reflection

#pragma once

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::reflection
{
    class Type;

    /// @brief Describes a constant version of another type.
    /// @ingroup core-reflection
    class CUBOS_CORE_API ConstantTrait
    {
    public:
        CUBOS_REFLECT;

        /// @brief Constructs.
        ConstantTrait(const Type& type)
            : mType(type)
        {
        }

        /// @brief Move constructs.
        ConstantTrait(ConstantTrait&& other) noexcept
            : mType(other.mType)
        {
        }

        /// @brief Gets the inner, non-constant, type.
        /// @return Type.
        const Type& type() const
        {
            return mType;
        }

    private:
        const Type& mType;
    };
} // namespace cubos::core::reflection
