/// @file
/// @brief Class @ref cubos::core::reflection::CategorizableTrait.
/// @ingroup core-reflection

#pragma once

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::reflection
{
    /// @brief Allows for a component to be hidden in the UI.
    /// @ingroup core-reflection
    class CUBOS_CORE_API HiddenTrait
    {

    public:
        CUBOS_REFLECT;

        /// @brief Constructs.
        /// @param hidden Hidden.
        HiddenTrait(bool hidden);

        /// @brief Gets the visibility of the type.
        /// @return Type visibility.
        bool hidden() const;

    private:
        bool mHidden;
    };

} // namespace cubos::core::reflection
