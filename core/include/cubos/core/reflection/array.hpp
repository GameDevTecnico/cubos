/// @file
/// @brief Defines the ArrayType class, which is an implementation of Type for array types.

#pragma once

#include <cubos/core/reflection/type.hpp>

namespace cubos::core::reflection
{
    /// @brief Implementation of Type for array types - types which contain a possibly variable
    /// number of elements of the same type.
    class ArrayType : public Type
    {
    public:
        /// @brief Function type for getting the length of an array.
        using Length = std::size_t (*)(const void*);

        /// @brief Function type for getting the address of an element of an array.
        using Element = uintptr_t (*)(const void*, std::size_t);

        /// @brief Function type for resizing an array.
        using Resize = void (*)(void*, std::size_t);

        /// @brief Builder for creating ArrayType's.
        class Builder : public Type::Builder
        {
        public:
            /// @brief Construct a new Builder object.
            /// @param type Type to build.
            Builder(ArrayType& type);

            /// @brief Sets the length getter function.
            /// @param length Length getter function.
            Builder& length(Length length);

            /// @brief Sets the element address getter function.
            /// @param element Element getter function.
            Builder& element(Element element);

            /// @brief Sets the resize function.
            /// @param resize Resize function.
            Builder& resize(Resize resize);

        private:
            ArrayType& mType;
        };

        /// @brief Creates a new Builder for an ArrayType with the given name and element type.
        /// @param name Name of the type.
        /// @param elementType Type of the elements of the array.
        /// @return Builder for the type.
        static Builder build(std::string name, const Type& elementType);

        /// @brief Gets the type of the elements of the array.
        /// @return Type of the elements of the array.
        const Type& elementType() const;

        /// @brief Gets the length getter function, or `nullptr` if not set.
        /// @return Length getter function.
        Length length() const;

        /// @brief Gets the element address getter function, or `nullptr` if not set.
        /// @return Element getter function.
        Element element() const;

        /// @brief Gets the resize function, or `nullptr` if not set.
        /// @return Resize function.
        Resize resize() const;

        /// @brief Gets the length of the array. Aborts if the length getter function is not set.
        /// The given array must be a valid instance of this type, otherwise, undefined behavior
        /// occurs.
        /// @param array Pointer to the array.
        /// @return Length of the array.
        std::size_t length(const void* array) const;

        /// @name Element access methods.
        /// @brief Gets the element at the given index. Aborts if the element getter function is
        /// not set. The given array must be a valid instance of this type, otherwise, undefined
        /// behavior occurs.
        /// @param array Pointer to the array.
        /// @param index Index of the element.
        /// @return Address of the element.
        /// @{
        const void* element(const void* array, std::size_t index) const;
        void* element(void* array, std::size_t index) const;
        /// @}

        /// @brief Resizes the array to the given length. Aborts if the resize function is not
        /// set. The given array must be a valid instance of this type, otherwise, undefined
        /// behavior occurs.
        /// @param array Pointer to the array.
        /// @param length New length of the array.
        void resize(void* array, std::size_t length) const;

        virtual void accept(class TypeVisitor& visitor) const override;

    private:
        ArrayType(std::string name, const Type& elementType);

        const Type& mElementType;
        Length mLength;
        Element mElement;
        Resize mResize;
    };
} // namespace cubos::core::reflection
