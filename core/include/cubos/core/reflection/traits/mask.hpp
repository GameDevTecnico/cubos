/// @file
/// @brief Class @ref cubos::core::reflection::MaskTrait.
/// @ingroup core-reflection

#pragma once

#include <string>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::reflection
{
    /// @brief Provides mask functionality to an enum mask type.
    /// @see See @ref examples-core-reflection-traits-mask for an example of using this trait.
    /// @ingroup core-reflection
    class MaskTrait
    {
    public:
        /// @brief Represents a bit within the mask type.
        class Bit;

        /// @brief Used to iterate over the bits on an enum trait.
        class Iterator;

        /// @brief Used to iterate over the set bits on a mask.
        class SetIterator;

        /// @brief Provides mutable access to a mask.
        class View;

        /// @brief Provides immutable access to a mask.
        class ConstView;

        /// @brief Function pointer to test if a bit is set on the mask.
        using Tester = bool (*)(const void* instance);

        /// @brief Function pointer to set the bit on the mask.
        using Setter = void (*)(void* instance);

        /// @brief Function pointer to clear the bit on the mask.
        using Clearer = void (*)(void* instance);

        ~MaskTrait();

        /// @brief Constructs.
        MaskTrait() = default;

        /// @brief Move constructs.
        /// @param other Other trait.
        MaskTrait(MaskTrait&& other) noexcept;

        /// @brief Adds a bit to the type.
        /// @param name Bit name.
        /// @param tester Function to test if a bit is set on the mask.
        /// @param setter Function to set a bit on the mask.
        /// @param clearer Function to clear a bit on the mask.
        void addBit(std::string name, Tester tester, Setter setter, Clearer clearer);

        /// @brief Adds a bit to the type.
        /// @param name Bit name.
        /// @param tester Function to test if a bit is set on the mask.
        /// @param setter Function to set a bit on the mask.
        /// @param clearer Function to clear a bit on the mask.
        /// @return Trait.
        MaskTrait&& withBit(std::string name, Tester tester, Setter setter, Clearer clearer) &&;

        /// @brief Adds a bit to the type.
        ///
        /// The bit type must implement at least the operators '&', '|' and '~'.
        ///
        /// @param name Bit name.
        /// @tparam B Bit value.
        /// @return Trait.
        template <auto B>
        MaskTrait&& withBit(std::string name) &&
        {
            return std::move(*this).withBit(
                std::move(name), [](const void* value) { return (*static_cast<const decltype(B)*>(value) & B) == B; },
                [](void* value) { *static_cast<decltype(B)*>(value) = *static_cast<const decltype(B)*>(value) | B; },
                [](void* value) { *static_cast<decltype(B)*>(value) = *static_cast<const decltype(B)*>(value) & ~B; });
        }

        /// @brief Checks if a bit with the given name exists.
        /// @param name Bit name.
        /// @return Whether the bit exists.
        bool contains(const std::string& name) const;

        /// @brief Gets the bit with the specified name.
        /// @note Aborts if the bit does not exist.
        /// @param name Bit name.
        /// @return Reference.
        const Bit& at(const std::string& name) const;

        /// @brief Gets an iterator to the first bit of the type.
        /// @return Iterator.
        Iterator begin() const;

        /// @brief Gets an iterator to the last bit of the type.
        /// @return Iterator.
        static Iterator end();

        /// @brief Returns how many bits there are in the trait.
        /// @return Bit count.
        std::size_t size() const;

        /// @brief Returns a view of the given mask instance.
        /// @param instance Mask instance.
        /// @return Mask view.
        View view(void* instance) const;

        /// @copydoc view(void*) const
        ConstView view(const void* instance) const;

    private:
        Bit* mFirstBit{nullptr};
        Bit* mLastBit{nullptr};
    };

    class MaskTrait::Bit
    {
    public:
        /// @brief Constructs.
        /// @param name Name.
        /// @param tester Function to test if a bit is set on the mask.
        /// @param setter Function to set a bit on the mask.
        /// @param clearer Function to clear a bit on the mask.
        Bit(std::string name, Tester tester, Setter setter, Clearer clearer);

        /// @brief Tests if the given value has the bit set.
        /// @param value Pointer to value.
        /// @return Whether the value has the bit set.
        bool test(const void* value) const;

        /// @brief Sets the bit on the given value.
        /// @param value Pointer to value.
        void set(void* value) const;

        /// @brief Clears the bit on the given value.
        /// @param value Pointer to value.
        void clear(void* value) const;

        /// @brief Gets the name of the bit.
        /// @return Bit name.
        const std::string& name() const;

        /// @brief Returns the next bit in the linked list.
        /// @return Pointer to next bit or null if this is the last bit.
        const Bit* next() const;

    private:
        friend MaskTrait;

        std::string mName;
        Bit* mNext{nullptr};

        Tester mTester;
        Setter mSetter;
        Clearer mClearer;
    };

    class MaskTrait::Iterator final
    {
    public:
        /// @brief Constructs.
        /// @param bit Bit.
        Iterator(const Bit* bit);

        /// @brief Copy constructs.
        /// @param iterator Iterator.
        Iterator(const Iterator& iterator) = default;

        /// @brief Copy assigns.
        /// @param iterator Iterator.
        /// @return This.
        Iterator& operator=(const Iterator&) = default;

        /// @brief Compares with another iterator.
        /// @param iterator Iterator.
        /// @return Whether both point to the same variant.
        bool operator==(const Iterator&) const = default;

        /// @brief Accesses the bit referenced by this iterator.
        /// @note Aborts if there's no bit.
        /// @return Reference to the bit.
        const Bit& operator*() const;

        /// @brief Accesses the bit referenced by this iterator.
        /// @note Aborts if there's no bit.
        /// @return Pointer to the bit.
        const Bit* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if there's no bit.
        /// @return Reference to this.
        Iterator& operator++();

    private:
        const Bit* mBit;
    };

    class MaskTrait::SetIterator
    {
    public:
        /// @brief Constructs.
        /// @param bit Bit to start from.
        /// @param instance Mask instance.
        SetIterator(const Bit* bit, const void* instance);

        /// @brief Copy constructs.
        /// @param other Other iterator.
        SetIterator(const SetIterator& other) = default;

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to the same bit.
        bool operator==(const SetIterator& other) const = default;

        /// @brief Accesses the bit referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Bit.
        const Bit& operator*() const;

        /// @brief Accesses the bit referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Bit.
        const Bit* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if out of bounds.
        /// @return Reference to this.
        SetIterator& operator++();

    private:
        const Bit* mBit;
        const void* mInstance;
    };

    class MaskTrait::View
    {
    public:
        /// @brief Constructs.
        /// @param trait Trait.
        /// @param instance Instance.
        View(const MaskTrait& trait, void* instance);

        /// @brief Checks if the given bit is set on the mask.
        /// @param bit Bit.
        /// @return Whether the bit is set.
        bool test(const Bit& bit) const;

        /// @brief Checks if the given bit is set on the mask.
        ///
        /// Aborts if the bit does not exist.
        ///
        /// @param name Bit name.
        /// @return Whether the bit is set.
        bool test(const std::string& name) const;

        /// @brief Sets the given bit on the mask.
        /// @param bit Bit.
        void set(const Bit& bit) const;

        /// @brief Sets the given bit on the mask.
        ///
        /// Aborts if the bit does not exist.
        ///
        /// @param name Bit name.
        void set(const std::string& name) const;

        /// @brief Clears the given bit on the mask.
        /// @param bit Bit.
        void clear(const Bit& bit) const;

        /// @brief Clears the given bit on the mask.
        ///
        /// Aborts if the bit does not exist.
        ///
        /// @param name Bit name.
        void clear(const std::string& name) const;

        /// @brief Gets an iterator to the first set bit.
        /// @return Iterator.
        SetIterator begin() const;

        /// @brief Gets an iterator representing the end of the set bits.
        /// @return Iterator.
        SetIterator end() const;

    private:
        const MaskTrait& mTrait;
        void* mInstance;
    };

    class MaskTrait::ConstView
    {
    public:
        /// @brief Constructs.
        /// @param trait Trait.
        /// @param instance Instance.
        ConstView(const MaskTrait& trait, const void* instance);

        /// @brief Checks if the given bit is set on the mask.
        /// @param bit Bit.
        /// @return Whether the bit is set.
        bool test(const Bit& bit) const;

        /// @brief Checks if the given bit is set on the mask.
        ///
        /// Aborts if the bit does not exist.
        ///
        /// @param name Bit name.
        /// @return Whether the bit is set.
        bool test(const std::string& name) const;

        /// @brief Gets an iterator to the first set bit.
        /// @return Iterator.
        SetIterator begin() const;

        /// @brief Gets an iterator representing the end of the set bits.
        /// @return Iterator.
        SetIterator end() const;

    private:
        const MaskTrait& mTrait;
        const void* mInstance;
    };
} // namespace cubos::core::reflection
