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

/// @name Declarations of reflect() for common primitive types.
/// @{
CUBOS_REFLECT_EXTERNAL_DECL(bool);
CUBOS_REFLECT_EXTERNAL_DECL(char);
CUBOS_REFLECT_EXTERNAL_DECL(int8_t);
CUBOS_REFLECT_EXTERNAL_DECL(int16_t);
CUBOS_REFLECT_EXTERNAL_DECL(int32_t);
CUBOS_REFLECT_EXTERNAL_DECL(int64_t);
CUBOS_REFLECT_EXTERNAL_DECL(uint8_t);
CUBOS_REFLECT_EXTERNAL_DECL(uint16_t);
CUBOS_REFLECT_EXTERNAL_DECL(uint32_t);
CUBOS_REFLECT_EXTERNAL_DECL(uint64_t);
CUBOS_REFLECT_EXTERNAL_DECL(float);
CUBOS_REFLECT_EXTERNAL_DECL(double);
CUBOS_REFLECT_EXTERNAL_DECL(std::string);
/// @}
