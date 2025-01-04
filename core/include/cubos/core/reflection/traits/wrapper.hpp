/// @file
/// @brief Class @ref cubos::core::reflection::WrapperTrait.
/// @ingroup core-reflection

#pragma once

#include <cstdint>
#include <string>

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/tel/logging.hpp>

namespace cubos::core::reflection
{
    /// @brief Describes the single field of a reflected type.
    /// @ingroup core-reflection
    class CUBOS_CORE_API WrapperTrait
    {
    public:
        CUBOS_REFLECT;

        ~WrapperTrait()
        {
            delete mAddressOf;
        };

        /// @brief Constructs.
        WrapperTrait(WrapperTrait&& other) noexcept
            : mType(other.mType)
            , mAddressOf(other.mAddressOf)
        {
            other.mAddressOf = nullptr;
        }

        /// @brief Constructs.
        template <typename F, typename O>
        WrapperTrait(F O::*pointer)
            : mType(reflect<F>())
            , mAddressOf(new FieldsTrait::AddressOfImpl<O, F>(pointer))
        {
        }

        /// @brief Gets the type of the single field.
        /// @return Type.
        const Type& type() const
        {
            return mType;
        }

        /// @brief Gets a pointer to the field on a given instance of the reflected type.
        /// @param instance Pointer to the instance.
        /// @return Pointer to the field on the given instance.
        void* value(const void* instance) const
        {
            return reinterpret_cast<void*>(this->mAddressOf->get(instance));
        }

    private:
        const Type& mType;
        const FieldsTrait::AddressOf* mAddressOf;
    };
} // namespace cubos::core::reflection