/// @file
/// @brief Class @ref cubos::core::reflection::ArrayTrait.
/// @ingroup core-reflection

#pragma once

#include <cstddef>
#include <cstdint>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::reflection
{
    /// @brief Exposes array-like functionality of a type.
    /// @see See @ref examples-core-reflection-traits-array for an example of using this trait.
    /// @ingroup core-reflection
    class ArrayTrait final
    {
    public:
        /// @brief Function pointer to get the length of an array instance.
        /// @param instance Pointer to the instance.
        using Length = std::size_t (*)(const void* instance);

        /// @brief Function pointer to get the address of an element in an array instance.
        /// @param instance Pointer to the instance.
        /// @param index Element index.
        using AddressOf = uintptr_t (*)(const void* instance, std::size_t index);

        /// @brief Function pointer to insert a default value into the array.
        /// @param instance Pointer to the instance.
        /// @param index Element index.
        using InsertDefault = void (*)(void* instance, std::size_t index);

        /// @brief Function pointer to insert a copy of the given value into the array.
        /// @param instance Pointer to the instance.
        /// @param index Element index.
        /// @param value Value.
        using InsertCopy = void (*)(void* instance, std::size_t index, const void* value);

        /// @brief Function pointer to move the given value into the array.
        /// @param instance Pointer to the instance.
        /// @param index Element index.
        /// @param value Value.
        using InsertMove = void (*)(void* instance, std::size_t index, void* value);

        /// @brief Function pointer to remove an element of the array.
        /// @param instance Pointer to the instance.
        /// @param index Element index.
        using Erase = void (*)(void* instance, std::size_t index);

        /// @brief Constructs.
        /// @param elementType Element type of the array.
        /// @param length Function used to get the length of an array.
        /// @param addressOf Function used to address an element in an array.
        ArrayTrait(const Type& elementType, Length length, AddressOf addressOf);

        /// @brief Sets the default-construct insert operation to the trait.
        /// @param insertDefault Function pointer.
        void insertDefault(InsertDefault insertDefault);

        /// @brief Sets the copy-construct insert operation to the trait.
        /// @param insertCopy Function pointer.
        void insertCopy(InsertCopy insertCopy);

        /// @brief Sets the move-construct insert operation to the trait.
        /// @param insertMove Function pointer.
        void insertMove(InsertMove insertMove);

        /// @brief Sets the erase operation to the trait.
        /// @param erase Function pointer.
        void erase(Erase erase);

        /// @brief Returns the element type of the array.
        /// @return Element type.
        const Type& elementType() const;

        /// @brief Returns the length of the given array.
        /// @param instance Pointer to the array instance.
        /// @return Array length.
        std::size_t length(const void* instance) const;

        /// @brief Gets a pointer to the element with the given index on the given array.
        /// @param instance Pointer to the array instance.
        /// @param index Element index.
        /// @return Pointer to the element.
        void* get(void* instance, std::size_t index) const;

        /// @copydoc get(void*, std::size_t) const
        const void* get(const void* instance, std::size_t index) const;

        /// @brief Inserts a default-constructed element into the array.
        /// @note Aborts if @ref hasInsertDefault() returns false.
        /// @param instance Pointer to the array instance.
        /// @param index Element index.
        /// @return Whether the operation is supported.
        bool insertDefault(void* instance, std::size_t index) const;

        /// @brief Inserts a copy-constructed element into the array.
        /// @note Aborts if @ref hasInsertCopy() returns false.
        /// @param instance Pointer to the array instance.
        /// @param index Element index.
        /// @param value Value.
        /// @return Whether the operation is supported.
        bool insertCopy(void* instance, std::size_t index, const void* value) const;

        /// @brief Inserts a move-constructed element into the array.
        /// @note Aborts if @ref hasInsertMove() returns false.
        /// @param instance Pointer to the array instance.
        /// @param index Element index.
        /// @param value Value.
        /// @return Whether the operation is supported.
        bool insertMove(void* instance, std::size_t index, void* value) const;

        /// @brief Removes an element of the array.
        /// @note Aborts if @ref hasErase() returns false.
        /// @param instance Pointer to the array instance.
        /// @param index Element index.
        /// @return Whether the operation is supported.
        bool erase(void* instance, std::size_t index) const;

        /// @brief Checks if default-construct insert is supported.
        /// @return Whether the operation is supported.
        bool hasInsertDefault() const;

        /// @brief Checks if copy-construct insert is supported.
        /// @return Whether the operation is supported.
        bool hasInsertCopy() const;

        /// @brief Checks if move-construct insert is supported.
        /// @return Whether the operation is supported.
        bool hasInsertMove() const;

        /// @brief Checks if erase is supported.
        /// @return Whether the operation is supported.
        bool hasErase() const;

    private:
        const Type& mElementType;
        Length mLength;
        AddressOf mAddressOf;
        InsertDefault mInsertDefault;
        InsertCopy mInsertCopy;
        InsertMove mInsertMove;
        Erase mErase;
    };
} // namespace cubos::core::reflection
