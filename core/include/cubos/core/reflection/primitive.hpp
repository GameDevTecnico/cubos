/// @file
/// Defines the PrimitiveType class, which is an implementation of Type for primitive types.

#pragma once

#include <vector>

#include <cubos/core/reflection/type.hpp>

namespace cubos::core::reflection
{
    /// @brief Implementation of Type for primitive types - types which cannot be further
    /// decomposed (e.g. `int`, `float`, `bool`,  `std::string`, etc.).
    class PrimitiveType : public Type
    {
    public:
        /// @brief Creates a new Builder for an PrimitiveType with the given name.
        /// @param name Name of the type.
        /// @return Builder for the type.
        static Builder build(std::string name);

        virtual void accept(class TypeVisitor& visitor) const override;

    private:
        PrimitiveType(std::string name);
    };
} // namespace cubos::core::reflection
