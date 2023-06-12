/// @file
/// @brief Defines the TypeVisitor interface class.

#pragma once

namespace cubos::core::reflection
{
    class PrimitiveType;
    class ObjectType;
    class VariantType;
    class ArrayType;
    class DictionaryType;

    /// @brief Interface class for visitors of types.
    class TypeVisitor
    {
    public:
        /// @brief Visit a primitive type.
        /// @param type Primitive type to visit.
        virtual void visit(const PrimitiveType& type) = 0;

        /// @brief Visit an object type.
        /// @param type Object type to visit.
        virtual void visit(const ObjectType& type) = 0;

        /// @brief Visit a variant type.
        /// @param type Variant type to visit.
        virtual void visit(const VariantType& type) = 0;

        /// @brief Visit an array type.
        /// @param type Array type to visit.
        virtual void visit(const ArrayType& type) = 0;

        /// @brief Visit a dictionary type.
        /// @param type Dictionary type to visit.
        virtual void visit(const DictionaryType& type) = 0;
    };
} // namespace cubos::core::reflection
