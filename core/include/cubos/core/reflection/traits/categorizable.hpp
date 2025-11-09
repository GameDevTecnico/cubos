/// @file
/// @brief Class @ref cubos::core::reflection::CategorizableTrait.
/// @ingroup core-reflection

#pragma once

#include <string>

#include <cubos/core/memory/function.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::reflection
{
    /// @brief Provides categorization functionality by assigning a category and a priority to a type. 
    /// @see See @ref examples-core-reflection-traits-categorizable for an example of using this trait.
    /// @ingroup core-reflection
    class CUBOS_CORE_API CategorizableTrait
    {
    public:
        CUBOS_REFLECT;

        /// @brief Constructs.
        /// @param category Category.
        /// @param priority Priority.
        CategorizableTrait(std::string category, std::size_t priority);

        /// @brief Gets the category of the type.
        /// @return Type category.
        std::string category() const;

        /// @brief Gets the priority of the type within its category.
        /// @return Type priority.
        std::size_t priority() const;

    private:
        std::string mCategory;
        std::size_t mPriority;
    };

} // namespace cubos::core::reflection
