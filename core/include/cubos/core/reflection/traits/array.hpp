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
    class CUBOS_CORE_API ArrayTrait final
    {
    public:
        CUBOS_REFLECT;

        /// @brief Provides mutable access to an array.
        class View;

        /// @brief Provides immutable access to an array.
        class ConstView;

        /// @brief Function pointer to get the length of an array instance.
        using Length = std::size_t (*)(const void* instance);

        /// @brief Function pointer to get the address of an element in an array instance.
        using AddressOf = uintptr_t (*)(const void* instance, std::size_t index);

        /// @brief Function pointer to insert a default value into the array.
        using InsertDefault = void (*)(void* instance, std::size_t index);

        /// @brief Function pointer to insert a copy of the given value into the array.
        using InsertCopy = void (*)(void* instance, std::size_t index, const void* value);

        /// @brief Function pointer to move the given value into the array.
        using InsertMove = void (*)(void* instance, std::size_t index, void* value);

        /// @brief Function pointer to remove an element of the array.
        using Erase = void (*)(void* instance, std::size_t index);

        /// @brief Constructs.
        /// @param length Function used to get the length of an array.
        /// @param addressOf Function used to address an element in an array.
        ArrayTrait(const Type& elementType, Length length, AddressOf addressOf);

        /// @brief Sets the default-construct insert operation to the trait.
        /// @param insertDefault Function pointer.
        void setInsertDefault(InsertDefault insertDefault);

        /// @brief Sets the copy-construct insert operation to the trait.
        /// @param insertCopy Function pointer.
        void setInsertCopy(InsertCopy insertCopy);

        /// @brief Sets the move-construct insert operation to the trait.
        /// @param insertMove Function pointer.
        void setInsertMove(InsertMove insertMove);

        /// @brief Sets the erase operation to the trait.
        /// @param erase Function pointer.
        void setErase(Erase erase);

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

        /// @brief Checks if resize is supported.
        /// @return Whether the operation is supported.
        bool hasResize() const;

        /// @brief Returns the element type of the array.
        /// @return Element type.
        const Type& elementType() const;

        /// @brief Returns a view of the given array instance.
        /// @param instance Array instance.
        /// @return Array view.
        View view(void* instance) const;

        /// @copydoc view(void*) const
        ConstView view(const void* instance) const;

    private:
        friend View;
        friend ConstView;

        const Type& mElementType;
        Length mLength;
        AddressOf mAddressOf;
        InsertDefault mInsertDefault{nullptr};
        InsertCopy mInsertCopy{nullptr};
        InsertMove mInsertMove{nullptr};
        Erase mErase{nullptr};
    };

    class CUBOS_CORE_API ArrayTrait::View final
    {
    public:
        /// @brief Used to iterate over the elements of an array.
        class Iterator;

        /// @brief Constructs.
        /// @param trait Trait.
        /// @param instance Instance.
        View(const ArrayTrait& trait, void* instance);

        /// @brief Returns the length of the array.
        /// @return Array length.
        std::size_t length() const;

        /// @brief Gets a pointer to the element with the given index.
        /// @param index Element index.
        /// @return Pointer to the element.
        void* get(std::size_t index) const;

        /// @brief Inserts a default-constructed element.
        /// @note Aborts if @ref ArrayTrait::hasInsertDefault() returns false.
        /// @param index Element index.
        void insertDefault(std::size_t index) const;

        /// @brief Inserts a copy-constructed element.
        /// @note Aborts if @ref ArrayTrait::hasInsertCopy() returns false.
        /// @param index Element index.
        /// @param value Value.
        void insertCopy(std::size_t index, const void* value) const;

        /// @brief Inserts a move-constructed element.
        /// @note Aborts if @ref ArrayTrait::hasInsertMove() returns false.
        /// @param index Element index.
        /// @param value Value.
        void insertMove(std::size_t index, void* value) const;

        /// @brief Removes an element.
        /// @note Aborts if @ref ArrayTrait::hasErase() returns false.
        /// @param index Element index.
        void erase(std::size_t index) const;

        /// @brief Clears the array.
        /// @note Aborts if @ref ArrayTrait::hasErase() returns false.
        void clear() const;

        /// @brief Resizes the array. If this increases the size of the array, @ref insertDefault
        /// is used to insert the remaining elements.
        /// @note Aborts if @ref ArrayTrait::hasResize() returns false.
        /// @param size Array size.
        void resize(std::size_t size) const;

        /// @brief Gets an iterator to the first element.
        /// @return Iterator.
        Iterator begin() const;

        /// @brief Gets an iterator to the element after the last element.
        /// @return Iterator.
        Iterator end() const;

    private:
        const ArrayTrait& mTrait;
        void* mInstance;
    };

    class CUBOS_CORE_API ArrayTrait::ConstView final
    {
    public:
        /// @brief Used to iterate over the elements of an array.
        class Iterator;

        /// @brief Constructs.
        /// @param trait Trait.
        /// @param instance Instance.
        ConstView(const ArrayTrait& trait, const void* instance);

        /// @brief Returns the length of the array.
        /// @return Array length.
        std::size_t length() const;

        /// @brief Gets a pointer to the element with the given index.
        /// @param index Element index.
        /// @return Pointer to the element.
        const void* get(std::size_t index) const;

        /// @brief Gets an iterator to the first element.
        /// @return Iterator.
        Iterator begin() const;

        /// @brief Gets an iterator to the element after the last element.
        /// @return Iterator.
        Iterator end() const;

    private:
        const ArrayTrait& mTrait;
        const void* mInstance;
    };

    class CUBOS_CORE_API ArrayTrait::View::Iterator final
    {
    public:
        /// @brief Constructs.
        /// @param view Array view.
        /// @param index Element index.
        Iterator(const View& view, std::size_t index);

        /// @brief Copy constructs.
        /// @param iterator Iterator.
        Iterator(const Iterator& iterator) = default;

        Iterator& operator=(const Iterator&) = default;
        bool operator==(const Iterator&) const = default;
        bool operator!=(const Iterator&) const = default;

        /// @brief Accesses the element referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Element.
        void* operator*() const;

        /// @brief Accesses the element referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Element.
        void* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if out of bounds.
        /// @return Reference to this.
        Iterator& operator++();

    private:
        const View* mView;
        std::size_t mIndex;
    };

    class CUBOS_CORE_API ArrayTrait::ConstView::Iterator final
    {
    public:
        /// @brief Constructs.
        /// @param view Array view.
        /// @param index Element index.
        Iterator(const ConstView& view, std::size_t index);

        /// @brief Copy constructs.
        /// @param iterator Iterator.
        Iterator(const Iterator& iterator) = default;

        Iterator& operator=(const Iterator&) = default;
        bool operator==(const Iterator&) const = default;
        bool operator!=(const Iterator&) const = default;

        /// @brief Accesses the element referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Element.
        const void* operator*() const;

        /// @brief Accesses the element referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Element.
        const void* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if out of bounds.
        /// @return Reference to this.
        Iterator& operator++();

    private:
        const ConstView* mView;
        std::size_t mIndex;
    };
} // namespace cubos::core::reflection
