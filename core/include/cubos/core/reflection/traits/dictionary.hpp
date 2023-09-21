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
    class DictionaryTrait final
    {
    public:
        /// @brief Points to a key-value pair in a dictionary, allowing modification of the value.
        class Iterator;

        /// @brief Points to a key-value pair in a dictionary.
        class ConstIterator;

        /// @brief Function pointer to get the length of a dictionary instance.
        /// @param instance Dictionary instance.
        /// @return Length.
        using Length = std::size_t (*)(const void* instance);

        /// @brief Function pointer to get an iterator to the first key-value pair of a dictionary
        /// instance.
        /// @note @ref Stop should be called on the returned iterator when it is no longer needed.
        /// @param instance Dictionary instance.
        /// @param writeable Whether the iterator should provide write access.
        /// @return Iterator.
        using Begin = void* (*)(uintptr_t instance, bool writeable);

        /// @brief Function pointer to get an iterator to a value in an dictionary instance.
        /// @note @ref Stop should be called on the returned iterator when it is no longer needed.
        /// @param instance Dictionary instance.
        /// @param key Key.
        /// @param writeable Whether the iterator should provide write access.
        /// @return Iterator to the found key-value pair, or null if not found.
        using Find = void* (*)(uintptr_t instance, const void* key, bool writeable);

        /// @brief Function pointer to advance an iterator.
        /// @param instance Dictionary instance.
        /// @param iterator Iterator.
        /// @param writeable Whether the iterator provides write access.
        /// @return Whether the iterator is still valid.
        using Advance = bool (*)(uintptr_t instance, void* iterator, bool writeable);

        /// @brief Function pointer to destroy an iterator instance.
        /// @param iterator Iterator to the key-value pair.
        /// @param writeable Whether the iterator provides write access.
        using Stop = void (*)(void* iterator, bool writeable);

        /// @brief Function pointer to get the address of the key pointed to by an iterator.
        /// @param iterator Iterator.
        /// @param writeable Whether the iterator provides write access.
        /// @return Key address.
        using Key = const void* (*)(const void* iterator, bool writeable);

        /// @brief Function pointer to get the address of the value pointed to by an iterator.
        /// @param iterator Iterator.
        /// @param writeable Whether the iterator provides write access.
        /// @return Value address.
        using Value = uintptr_t (*)(const void* iterator, bool writeable);

        /// @brief Function pointer to insert a default value into a dictionary instance.
        /// @param instance Dictionary instance.
        /// @param key Key.
        using InsertDefault = void (*)(void* instance, const void* key);

        /// @brief Function pointer to insert a copy of the given value into a dictionary instance.
        /// @param instance Dictionary instance.
        /// @param key Key.
        /// @param value Value.
        using InsertCopy = void (*)(void* instance, const void* key, const void* value);

        /// @brief Function pointer to move the given value into a dictionary instance.
        /// @param instance Dictionary instance.
        /// @param key Key.
        /// @param value Value.
        using InsertMove = void (*)(void* instance, const void* key, void* value);

        /// @brief Function pointer to remove a key-value pair of a dictionary instance.
        /// @param instance Dictionary instance.
        /// @param iterator Iterator to the key-value pair.
        /// @param writeable Whether the iterator provides write access.
        using Erase = void (*)(void* instance, const void* iterator, bool writeable);

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

        /// @brief Returns the key type of the dictionary.
        /// @return Key type.
        const Type& keyType() const;

        /// @brief Returns the value type of the dictionary.
        /// @return Value type.
        const Type& valueType() const;

        /// @brief Returns the length of the given dictionary.
        /// @param instance Dictionary instance.
        /// @return Dictionary length.
        std::size_t length(const void* instance) const;

        /// @brief Returns an iterator to the beginning of the given dictionary.
        /// @param instance Dictionary instance.
        /// @return Iterator.
        Iterator begin(void* instance) const;

        /// @copydoc begin(void*) const
        ConstIterator begin(const void* instance) const;

        /// @brief Returns an iterator to the element of the given dictionary with the given key.
        /// @param instance Dictionary instance.
        /// @param key Key.
        /// @return Iterator to the found key-value pair, or null if not found.
        Iterator find(void* instance, const void* key) const;

        /// @copydoc find(void*, const void*) const
        ConstIterator find(const void* instance, const void* key) const;

        /// @brief Inserts a default-constructed value into the dicitonary.
        /// @param instance Dictionary instance.
        /// @param key Key.
        /// @return Whether the operation is supported.
        bool insertDefault(void* instance, const void* key) const;

        /// @brief Inserts a copy-constructed value into the dictionary.
        /// @param instance Dictionary instance.
        /// @param key Key.
        /// @param value Value.
        /// @return Whether the operation is supported.
        bool insertCopy(void* instance, const void* key, const void* value) const;

        /// @brief Inserts a move-constructed value into the dictionary.
        /// @param instance Dictionary instance.
        /// @param key Key.
        /// @param value Value.
        /// @return Whether the operation is supported.
        bool insertMove(void* instance, const void* key, void* value) const;

        /// @brief Removes a key-value pair of the dictionary.
        /// @param instance Dictionary instance.
        /// @param key Iterator.
        /// @return Whether the operation is supported.
        bool erase(void* instance, Iterator& iterator) const;

        /// @copydoc erase(void*, Iterator&) const
        bool erase(void* instance, ConstIterator& iterator) const;

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
        friend Iterator;

        const Type& mKeyType;
        const Type& mValueType;
        Length mLength;
        Begin mBegin;
        Find mFind;
        Advance mAdvance;
        Stop mStop;
        Key mKey;
        Value mValue;
        InsertDefault mInsertDefault;
        InsertCopy mInsertCopy;
        InsertMove mInsertMove;
        Erase mErase;
    };

    class DictionaryTrait::Iterator
    {
    public:
        ~Iterator();

        /// @brief Copy constructs.
        /// @param other Other iterator.
        Iterator(const Iterator& other);

        /// @brief Move constructs.
        /// @param other Other iterator.
        Iterator(Iterator&& other);

        /// @brief Gets a pointer to the current key.
        /// @note Aborts if @ref isNull() returns true.
        /// @return Key.
        const void* key() const;

        /// @brief Gets a pointer to the current value.
        /// @note Aborts if @ref isNull() returns true.
        /// @return Value.
        void* value() const;

        /// @brief Advances the iterator.
        /// @note Aborts if @ref isNull() returns true.
        /// @return Whether the iterator is still valid.
        bool advance();

        /// @brief Checks if the iterator is null.
        /// @return Whether the iterator is null.
        bool isNull() const;

    private:
        friend DictionaryTrait;

        /// @brief Constructs.
        /// @param inner Inner iterator.
        /// @param instance Dictionary instance.
        /// @param trait Dictionary trait.
        Iterator(void* inner, void* instance, const DictionaryTrait& trait);

        void* mInner;
        void* mInstance;
        const DictionaryTrait& mTrait;
    };

    class DictionaryTrait::ConstIterator
    {
    public:
        ~ConstIterator();

        /// @brief Copy constructs.
        /// @param other Other iterator.
        ConstIterator(const ConstIterator& other);

        /// @brief Move constructs.
        /// @param other Other iterator.
        ConstIterator(ConstIterator&& other);

        /// @brief Gets a pointer to the current key.
        /// @note Aborts if @ref isNull() returns true.
        /// @return Key.
        const void* key() const;

        /// @brief Gets a pointer to the current value.
        /// @note Aborts if @ref isNull() returns true.
        /// @return Value.
        const void* value() const;

        /// @brief Advances the iterator.
        /// @note Aborts if @ref isNull() returns true.
        /// @return Whether the iterator is still valid.
        bool advance();

        /// @brief Checks if the iterator is null.
        /// @return Whether the iterator is null.
        bool isNull() const;

    private:
        friend DictionaryTrait;

        /// @brief Constructs.
        /// @param inner Inner iterator.
        /// @param instance Dictionary instance.
        /// @param trait Dictionary trait.
        ConstIterator(void* inner, const void* instance, const DictionaryTrait& trait);

        void* mInner;
        const void* mInstance;
        const DictionaryTrait& mTrait;
    };
} // namespace cubos::core::reflection
