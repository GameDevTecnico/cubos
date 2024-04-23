/// @file
/// @brief Class @ref cubos::core::reflection::DictionaryTrait.
/// @ingroup core-reflection

#pragma once

#include <cstddef>
#include <cstdint>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::reflection
{
    /// @brief Exposes dictionary-like functionality of a type.
    /// @see See @ref examples-core-reflection-traits-dictionary for an example of using this trait.
    /// @ingroup core-reflection
    class CUBOS_CORE_API DictionaryTrait final
    {
    public:
        CUBOS_REFLECT;

        /// @brief Provides mutable access to a dictionary.
        class View;

        /// @brief Provides immutable access to a dictionary.
        class ConstView;

        /// @brief Points to a key-value pair in a dictionary, allowing modification of the value.
        class Iterator;

        /// @brief Points to a key-value pair in a dictionary.
        class ConstIterator;

        /// @brief Function pointer to get the length of a dictionary instance.
        using Length = std::size_t (*)(const void* instance);

        /// @brief Function pointer to get an iterator to the first key-value pair of a dictionary
        /// instance.
        using Begin = void* (*)(uintptr_t instance, bool writeable);

        /// @brief Function pointer to get an iterator to a value in an dictionary instance.
        using Find = void* (*)(uintptr_t instance, const void* key, bool writeable);

        /// @brief Function pointer to advance an iterator.
        using Advance = bool (*)(uintptr_t instance, void* iterator, bool writeable);

        /// @brief Function pointer to destroy an iterator instance.
        using Stop = void (*)(void* iterator, bool writeable);

        /// @brief Function pointer to get the address of the key pointed to by an iterator.
        using Key = const void* (*)(const void* iterator, bool writeable);

        /// @brief Function pointer to get the address of the value pointed to by an iterator.
        using Value = uintptr_t (*)(const void* iterator, bool writeable);

        /// @brief Function pointer to insert a default value into a dictionary instance.
        using InsertDefault = void (*)(void* instance, const void* key);

        /// @brief Function pointer to insert a copy of the given value into a dictionary instance.
        using InsertCopy = void (*)(void* instance, const void* key, const void* value);

        /// @brief Function pointer to move the given value into a dictionary instance.
        using InsertMove = void (*)(void* instance, const void* key, void* value);

        /// @brief Function pointer to remove a key-value pair of a dictionary instance.
        using Erase = void (*)(void* instance, const void* iterator);

        /// @brief Constructs.
        /// @param keyType Key type of the dictionary.
        /// @param valueType Value type of the dictionary.
        /// @param length Function used to get the length of a dictionary.
        /// @param begin Function used to get an iterator to the first key-value pair of a
        /// @param find Function used to find a key-value pair in a dictionary.
        /// @param advance Function used to advance an iterator.
        /// @param stop Function used to destroy an iterator.
        /// @param key Function used to get the address of the key pointed to by an iterator.
        /// @param value Function used to get the address of the value pointed to by an iterator.
        DictionaryTrait(const Type& keyType, const Type& valueType, Length length, Begin begin, Find find,
                        Advance advance, Stop stop, Key key, Value value);

        /// @brief Sets the default-construct insert operation of the trait.
        /// @param insertDefault Function pointer.
        void setInsertDefault(InsertDefault insertDefault);

        /// @brief Sets the copy-construct insert operation of the trait.
        /// @param insertCopy Function pointer.
        void setInsertCopy(InsertCopy insertCopy);

        /// @brief Sets the move-construct insert operation of the trait.
        /// @param insertMove Function pointer.
        void setInsertMove(InsertMove insertMove);

        /// @brief Sets the erase operation of the trait.
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

        /// @brief Returns the key type of the dictionary.
        /// @return Key type.
        const Type& keyType() const;

        /// @brief Returns the value type of the dictionary.
        /// @return Value type.
        const Type& valueType() const;

        /// @brief Returns a view of the given dictionary instance.
        /// @param instance Dictionary instance.
        /// @return Dictionary view.
        View view(void* instance) const;

        /// @copydoc view(void*) const
        ConstView view(const void* instance) const;

    private:
        friend View;
        friend ConstView;

        const Type& mKeyType;
        const Type& mValueType;
        Length mLength;
        Begin mBegin;
        Find mFind;
        Advance mAdvance;
        Stop mStop;
        Key mKey;
        Value mValue;
        InsertDefault mInsertDefault{nullptr};
        InsertCopy mInsertCopy{nullptr};
        InsertMove mInsertMove{nullptr};
        Erase mErase{nullptr};
    };

    class CUBOS_CORE_API DictionaryTrait::View
    {
    public:
        /// @brief Used to iterate over the entries of a dictionary.
        class Iterator;

        /// @brief Constructs.
        /// @param trait Trait.
        /// @param instance Instance.
        View(const DictionaryTrait& trait, void* instance);

        /// @brief Returns the length of the dictionary.
        /// @return Dictionary length.
        std::size_t length() const;

        /// @brief Returns an iterator to the first entry.
        /// @return Iterator.
        Iterator begin() const;

        /// @brief Returns an iterator to the entry after the last entry.
        /// @return Iterator.
        Iterator end() const;

        /// @brief Returns an iterator to the entry with the given key.
        /// @note If no entry with the given key exists, @ref end() is returned.
        /// @param key Key.
        /// @return Iterator.
        Iterator find(const void* key) const;

        /// @brief Inserts a default-constructed value with the given key.
        /// @note Aborts if @ref DictionaryTrait::hasInsertDefault() returns false.
        /// @param key Key.
        void insertDefault(const void* key) const;

        /// @brief Inserts a copy-constructed value with the given key.
        /// @note Aborts if @ref DictionaryTrait::hasInsertCopy() returns false.
        /// @param key Key.
        /// @param value Value.
        void insertCopy(const void* key, const void* value) const;

        /// @brief Inserts a move-constructed value with the given key.
        /// @note Aborts if @ref DictionaryTrait::hasInsertMove() returns false.
        /// @param key Key.
        /// @param value Value.
        void insertMove(const void* key, void* value) const;

        /// @brief Removes an entry.
        /// @note Aborts if @ref DictionaryTrait::hasErase() returns false.
        /// @param iterator Iterator.
        void erase(Iterator& iterator) const;

        /// @copydoc erase(Iterator&) const
        void erase(Iterator&& iterator) const;

        /// @brief Clears the dictionary.
        /// @note Aborts if @ref DictionaryTrait::hasErase() returns false.
        void clear() const;

    private:
        const DictionaryTrait& mTrait;
        void* mInstance;
    };

    class CUBOS_CORE_API DictionaryTrait::ConstView
    {
    public:
        /// @brief Used to iterate over the entries of a dictionary.
        class Iterator;

        /// @brief Constructs.
        /// @param trait Trait.
        /// @param instance Instance.
        ConstView(const DictionaryTrait& trait, const void* instance);

        /// @brief Returns the length of the dictionary.
        /// @return Dictionary length.
        std::size_t length() const;

        /// @brief Returns an iterator to the first entry.
        /// @return Iterator.
        Iterator begin() const;

        /// @brief Returns an iterator to the entry after the last entry.
        /// @return Iterator.
        Iterator end() const;

        /// @brief Returns an iterator to the entry with the given key.
        /// @note If no entry with the given key exists, @ref end() is returned.
        /// @param key Key.
        /// @return Iterator.
        Iterator find(const void* key) const;

    private:
        const DictionaryTrait& mTrait;
        const void* mInstance;
    };

    class CUBOS_CORE_API DictionaryTrait::View::Iterator
    {
    public:
        /// @brief Output structure for the iterator.
        struct Entry
        {
            const void* key{nullptr}; ///< Key.
            void* value{nullptr};     ///< Value.
        };

        ~Iterator();

        /// @brief Copy constructs.
        /// @param other Other iterator.
        Iterator(const Iterator& other);

        /// @brief Move constructs.
        /// @param other Other iterator.
        Iterator(Iterator&& other) noexcept;

        /// @brief Assigns an iterator.
        /// @param other Other iterator.
        /// @return Iterator.
        Iterator& operator=(const Iterator& other);

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to the same entry.
        bool operator==(const Iterator& other) const;

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to different entries.
        bool operator!=(const Iterator& /*other*/) const;

        /// @brief Accesses the entry referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Entry.
        const Entry& operator*() const;

        /// @brief Accesses the entry referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Entry.
        const Entry* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if out of bounds.
        /// @return Reference to this.
        Iterator& operator++();

    private:
        friend DictionaryTrait;

        /// @brief Constructs.
        /// @param view View.
        /// @param inner Inner iterator.
        Iterator(const View& view, void* inner);

        const View& mView;
        void* mInner{nullptr};
        mutable Entry mEntry;
    };

    class CUBOS_CORE_API DictionaryTrait::ConstView::Iterator
    {
    public:
        /// @brief Output structure for the iterator.
        struct Entry
        {
            const void* key{nullptr};   ///< Key.
            const void* value{nullptr}; ///< Value.
        };

        ~Iterator();

        /// @brief Copy constructs.
        /// @param other Other iterator.
        Iterator(const Iterator& other);

        /// @brief Move constructs.
        /// @param other Other iterator.
        Iterator(Iterator&& other) noexcept;

        /// @brief Assigns an iterator.
        /// @param other Other iterator.
        /// @return Iterator.
        Iterator& operator=(const Iterator& other);

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to the same entry.
        bool operator==(const Iterator& other) const;

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to different entries.
        bool operator!=(const Iterator& /*other*/) const;

        /// @brief Accesses the entry referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Entry.
        const Entry& operator*() const;

        /// @brief Accesses the entry referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Entry.
        const Entry* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if out of bounds.
        /// @return Reference to this.
        Iterator& operator++();

    private:
        friend DictionaryTrait;

        /// @brief Constructs.
        /// @param view View.
        /// @param inner Inner iterator.
        Iterator(const ConstView& view, void* inner);

        const ConstView& mView;
        void* mInner{nullptr};
        mutable Entry mEntry;
    };
} // namespace cubos::core::reflection
