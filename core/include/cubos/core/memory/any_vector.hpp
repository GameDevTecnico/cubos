/// @file
/// @brief Class @ref cubos::core::memory::AnyVector.
/// @ingroup core-memory

#pragma once

#include <cstddef>

#include <cubos/core/api.hpp>

namespace cubos::core::reflection
{
    class Type;
    class ConstructibleTrait;
} // namespace cubos::core::reflection

namespace cubos::core::memory
{
    /// @brief Stores a dynamically sized array of blobs of a given reflected type.
    /// @ingroup core-memory
    class CUBOS_CORE_API AnyVector final
    {
    public:
        ~AnyVector();

        /// @brief Constructs with the given element type.
        /// @note @p elementType must have @ref reflection::ConstructibleTrait and be
        /// move constructible.
        /// @param elementType Element type.
        AnyVector(const reflection::Type& elementType);

        /// @brief Copy constructs.
        /// @note Aborts if the element type of @p other is not copy constructible.
        /// @param other Vector.
        AnyVector(const AnyVector& other);

        /// @brief Move constructs.
        /// @param other Vector.
        AnyVector(AnyVector&& other) noexcept;

        /// @brief Get the type of the elements stored in the vector.
        /// @return Element type.
        const reflection::Type& elementType() const;

        /// @brief Reserves space for at least @p capacity elements.
        /// @note Aborts if @p capacity is less than @ref size().
        /// @param capacity Minimum capacity.
        void reserve(std::size_t capacity);

        /// @brief Inserts an uninitialized element at the given index.
        /// @note Aborts if @p index is out of bounds.
        /// @param index Element index.
        void insertUninit(std::size_t index);

        /// @brief Inserts a default-constructed element at the given index.
        /// @note Aborts if @ref elementType() is not default-constructible.
        /// @note Aborts if @p index is out of bounds.
        /// @param index Element index.
        void insertDefault(std::size_t index);

        /// @brief Inserts a copy-constructed element at the given index.
        /// @note Aborts if @ref elementType() is not copy-constructible.
        /// @note Aborts if @p index is out of bounds.
        /// @param index Element index.
        /// @param value Element to copy.
        void insertCopy(std::size_t index, const void* value);

        /// @brief Inserts a move-constructed element at the given index.
        /// @note Aborts if @p index is out of bounds.
        /// @param index Element index.
        /// @param value Element to move.
        void insertMove(std::size_t index, void* value);

        /// @brief Pushes an uninitialized element to the back of the vector.
        /// @warning Care must be taken to ensure that the constructor of the element is called before it is used or
        /// before the vector is destroyed, as the destructor will be called on it.
        void pushUninit();

        /// @brief Pushes a new default-constructed element to the back of the vector.
        /// @note Aborts if @ref elementType() is not default-constructible.
        void pushDefault();

        /// @brief Pushes a new copy-constructed element to the back of the vector.
        /// @note Aborts if @ref elementType() is not copy-constructible.
        /// @param value Element to copy.
        void pushCopy(const void* value);

        /// @brief Pushes a new move-constructed element to the back of the vector.
        /// @param value Element to move.
        void pushMove(void* value);

        /// @brief Overwrites the element at the given index with a new default-constructed element.
        /// @note Aborts if @ref elementType() is not default-constructible.
        /// @param index Element index.
        void setDefault(std::size_t index);

        /// @brief Overwrites the element at the given index with a new copy-constructed element.
        /// @note Aborts if @ref elementType() is not copy-constructible.
        /// @param index Element index.
        /// @param value Element to copy.
        void setCopy(std::size_t index, const void* value);

        /// @brief Overwrites the element at the given index with a new move-constructed element.
        /// @note Aborts if @ref elementType() is not move-constructible.
        /// @param index Element index.
        /// @param value Element to move.
        void setMove(std::size_t index, void* value);

        /// @brief Removes the last element from the vector.
        /// @note Aborts if the vector is empty.
        void pop();

        /// @brief Removes the element at the given index from the vector.
        ///
        /// Prefer using @ref swapErase() if the order of elements doesn't matter, as it is more efficient.
        ///
        /// @note Aborts if @p index is out of bounds.
        /// @param index Element index.
        void erase(std::size_t index);

        /// @brief Removes the element at the given index from the vector, moving the last element
        /// to its place if it isn't the last element.
        /// @note Aborts if @p index is out of bounds.
        /// @param index Element index.
        void swapErase(std::size_t index);

        /// @brief Moves the element at the given index from the vector into the given address, moving
        /// the last element to its place if it isn't the last element.
        /// @note Aborts if @p index is out of bounds.
        /// @param index Element index.
        /// @param destination Address to move the element to.
        void swapMove(std::size_t index, void* destination);

        /// @brief Removes all elements from the vector.
        void clear();

        /// @brief Get the element at the given index.
        /// @note Aborts if @p index is out of bounds.
        /// @param index Index of the element to get.
        /// @return Pointer to the element.
        void* at(std::size_t index);

        /// @copydoc at(std::size_t)
        const void* at(std::size_t index) const;

        /// @brief Get the number of elements in the vector.
        /// @return Element count.
        std::size_t size() const;

        /// @brief Get the number of elements the vector can hold without reallocating.
        /// @return Element capacity.
        std::size_t capacity() const;

        /// @brief Checks if the vector is empty.
        /// @return Whether the vector is empty.
        bool empty() const;

    private:
        /// @brief Moves a slice of elements from one position to another in the vector.
        ///
        /// No deconstructors are called on the elements which were moved into.
        ///
        /// @param from Start index of the slice to move.
        /// @param to Start index to move the slice to.
        /// @param count Number of elements to move.
        void moveSlice(std::size_t from, std::size_t to, std::size_t count);

        const reflection::Type& mElementType;
        const reflection::ConstructibleTrait* mConstructibleTrait{nullptr};
        void* mData{nullptr};
        std::size_t mSize{0};
        std::size_t mCapacity{0};
        std::size_t mStride{0};
    };
} // namespace cubos::core::memory
