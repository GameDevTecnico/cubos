/// @file
/// @brief Class @ref cubos::core::reflection::NullableTrait.
/// @ingroup core-reflection

#pragma once

#include <cubos/core/memory/function.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::reflection
{
    /// @brief Used to manipulate values of null-representable types.
    /// @see See @ref examples-core-reflection-traits-nullable for an example of using this trait.
    /// @ingroup core-reflection
    class CUBOS_CORE_API NullableTrait
    {
    public:
        CUBOS_REFLECT;

        /// @brief Function pointer to check if a value represents null.
        using IsNull = memory::Function<bool(const void*) const>;

        /// @brief Function pointer to set a value to its null representation.
        using SetToNull = memory::Function<void(void*) const>;

        /// @brief Constructs.
        /// @param isNull IsNull.
        /// @param setToNull SetToNull.
        NullableTrait(IsNull isNull, SetToNull setToNull);

        /// @brief Checks if the given value represents null.
        /// @param instance Instance.
        /// @return Whether the value represents null.
        bool isNull(const void* instance) const;

        /// @brief Sets a value to its null representation.
        /// @param instance Instance.
        void setToNull(void* instance) const;

    private:
        IsNull mIsNull;
        SetToNull mSetToNull;
    };
} // namespace cubos::core::reflection
