/// @file
/// @brief Defines the DictionaryType class, which is an implementation of Type for dictionary types.

#pragma once

#include <cubos/core/reflection/type.hpp>

namespace cubos::core::reflection
{
    /// @brief Implementation of Type for dictionary types - types which contain a possibly variable
    /// number of elements of the same type.
    class DictionaryType : public Type
    {
    public:
        /// @brief Function type for getting the length of an dictionary.
        using Length = std::size_t (*)(const void*);

        /// @brief Function type for getting a pointer to the value with the given key. Should
        /// return `0` if the key does not exist.
        using Value = uintptr_t (*)(const void*, const void*);

        /// @brief Function type for inserting a new default-constructed value into the dictionary
        /// with the given key. Returns a pointer to the inserted value. Should return the existing
        /// value if the key already exists.
        using Insert = void* (*)(void*, const void*);

        /// @brief Function type for removing the value with the given key. Should return `true`
        /// if the value was removed, and `false` if the key did not exist.
        using Remove = bool (*)(void*, const void*);

        /// @brief Function type for clearing the dictionary.
        using Clear = void (*)(void*);

        /// @brief Function type for creating an iterator for the dictionary. Returns `nullptr`
        /// if the dictionary is empty.
        using IteratorNew = void* (*)(const void*);

        /// @brief Function type for getting the key of the given iterator.
        using IteratorKey = const void* (*)(const void*, const void*);

        /// @brief Function type for getting the value of the given iterator.
        using IteratorValue = uintptr_t (*)(const void*, const void*);

        /// @brief Function type for incrementing the given iterator. Returns `true` if the
        /// iterator is at the end of the dictionary after incrementing, in which case the
        /// iterator is freed by the function.
        using IteratorIncrement = bool (*)(const void*, void*);

        /// @brief Function type for freeing an iterator.
        using IteratorDelete = void (*)(void*);

        /// @brief Used to iterate over an instance of any dictionary type.
        class Iterator final
        {
        public:
            /// @brief Construct a new iterator for the given dictionary type and instance.
            /// @param type Type of the dictionary.
            /// @param dictionary Instance of the dictionary.
            Iterator(const DictionaryType& type, const void* dictionary);
            ~Iterator();

            /// @brief Gets the key which the iterator is currently pointing to. Aborts if the
            /// iterator is at the end of the dictionary.
            /// @param dictionary Instance of the dictionary.
            /// @return Key which the iterator is currently pointing to.
            const void* key(const void* dictionary) const;

            /// @brief Gets the key which the iterator is currently pointing to. Aborts if the
            /// iterator is at the end of the dictionary. If K does not match the key type of the
            /// dictionary, the behaviour is undefined.
            /// @tparam K Type of the key.
            /// @param dictionary Instance of the dictionary.
            /// @return Key which the iterator is currently pointing to.
            template <typename K>
            const K& key(const void* dictionary) const
            {
                return *static_cast<const K*>(this->key(dictionary));
            }

            /// @name Getters for the value which the iterator is currently pointing to.
            /// @brief Gets the value which the iterator is currently pointing to. Aborts if the
            /// iterator is at the end of the dictionary.
            /// @param dictionary Instance of the dictionary.
            /// @return Value which the iterator is currently pointing to.
            /// @{
            const void* value(const void* dictionary) const;
            void* value(void* dictionary) const;
            /// @}

            /// @name Template getters for the value which the iterator is currently pointing to.
            /// @brief Gets the value which the iterator is currently pointing to. Aborts if the
            /// iterator is at the end of the dictionary. If T does not match the value type of the
            /// dictionary, the behaviour is undefined.
            /// @tparam V Type of the value.
            /// @param dictionary Instance of the dictionary.
            /// @return Value which the iterator is currently pointing to.
            /// @{
            template <typename V>
            const V& value(const void* dictionary) const
            {
                return *static_cast<const V*>(this->value(dictionary));
            }

            template <typename V>
            V& value(void* dictionary) const
            {
                return *static_cast<V*>(this->value(dictionary));
            }
            /// @}

            /// @brief Increments the iterator.
            /// @param dictionary Instance of the dictionary.
            void increment(const void* dictionary);

            /// @brief Checks if the iterator is pointing to a valid element.
            /// @return `true` if the iterator is pointing to a valid element, `false` otherwise.
            bool valid() const;

        private:
            const DictionaryType& mType;
            void* mIterator;
        };

        /// @brief Builder for creating DictionaryType's.
        class Builder : public Type::Builder
        {
        public:
            /// @brief Construct a new Builder object.
            /// @param type Type to build.
            Builder(DictionaryType& type);

            /// @brief Sets the length getter function.
            /// @param length Length getter function.
            Builder& length(Length length);

            /// @brief Sets the value address getter function.
            /// @param element Value getter function.
            Builder& value(Value value);

            /// @brief Sets the insert function.
            /// @param insert Insert function.
            Builder& insert(Insert insert);

            /// @brief Sets the remove function.
            /// @param remove Remove function.
            Builder& remove(Remove remove);

            /// @brief Sets the clear function.
            /// @param clear Clear function.
            Builder& clear(Clear clear);

            /// @brief Sets the iterator creation function.
            /// @param iterator Iterator creation function.
            Builder& iteratorNew(IteratorNew iterator);

            /// @brief Sets the iterator key getter function.
            /// @param iteratorKey Iterator key getter function.
            Builder& iteratorKey(IteratorKey iteratorKey);

            /// @brief Sets the iterator value getter function.
            /// @param iteratorValue Iterator value getter function.
            Builder& iteratorValue(IteratorValue iteratorValue);

            /// @brief Sets the iterator increment function.
            /// @param iteratorIncrement Iterator increment function.
            Builder& iteratorIncrement(IteratorIncrement iteratorIncrement);

            /// @brief Sets the iterator delete function.
            /// @param iteratorFree Iterator delete function.
            Builder& iteratorDelete(IteratorDelete iteratorDelete);

        private:
            DictionaryType& mType;
        };

        /// @brief Creates a new Builder for an DictionaryType with the given name and key and
        /// value types.
        /// @param name Name of the type.
        /// @param keyType Type of the keys of the dictionary.
        /// @param valueType Type of the values of the dictionary.
        /// @return Builder for the type.
        static Builder build(std::string name, const Type& keyType, const Type& valueType);

        /// @brief Gets the type of the keys of the dictionary.
        /// @return Type of the keys of the dictionary.
        const Type& keyType() const;

        /// @brief Gets the type of the values of the dictionary.
        /// @return Type of the values of the dictionary.
        const Type& valueType() const;

        /// @brief Gets the length getter function, or `nullptr` if not set.
        /// @return Length getter function.
        Length length() const;

        /// @brief Gets the value address getter function, or `nullptr` if not set.
        /// @return Value getter function.
        Value value() const;

        /// @brief Gets the insert function, or `nullptr` if not set.
        /// @return Insert function.
        Insert insert() const;

        /// @brief Gets the remove function, or `nullptr` if not set.
        /// @return Remove function.
        Remove remove() const;

        /// @brief Gets the clear function, or `nullptr` if not set.
        /// @return Clear function.
        Clear clear() const;

        /// @brief Checks if the type supports iteration.
        /// @return `true` if the type supports iteration, `false` otherwise.
        bool hasIterator() const;

        /// @brief Gets the length of the dictionary. Aborts if the length getter function is not
        /// set. The given pointer must be a valid instance of this type, otherwise, undefined
        /// behavior occurs.
        /// @param dictionary Pointer to the dictionary.
        /// @return Length of the dictionary.
        std::size_t length(const void* dictionary) const;

        /// @name Value access methods.
        /// @brief Gets the value with the given key, or `nullptr` if the key does not exist.
        /// Aborts if the value getter function is not set. The given dictionary must be a valid
        /// instance of this type, otherwise, undefined behavior occurs.
        /// @param dictionary Pointer to the dictionary.
        /// @param key Instance of the key type to get the value for.
        /// @return Pointer to the value, or `nullptr` if the key does not exist.
        /// @{
        const void* value(const void* dictionary, const void* key) const;
        void* value(void* dictionary, const void* key) const;
        /// @}

        /// @name Templated value access methods.
        /// @brief Gets the value with the given key, or `nullptr` if the key does not exist.
        /// Aborts if the value getter function is not set. The given dictionary must be a valid
        /// instance of this type and K and V must match the key and value types of the dictionary,
        /// otherwise, undefined behaviour occurs.
        /// @tparam K Type of the keys.
        /// @tparam V Type of the values.
        /// @param dictionary Pointer to the dictionary.
        /// @param key Instance of the key to get the value for.
        /// @return Pointer to the vlaue, or `nullptr` if the key does not exist.
        /// @{
        template <typename K, typename V>
        const V* value(const void* dictionary, const K& key) const
        {
            return static_cast<const V*>(this->value(dictionary, &key));
        }

        template <typename K, typename V>
        V* value(void* dictionary, const K& key) const
        {
            return static_cast<V*>(this->value(dictionary, &key));
        }
        /// @}

        /// @brief Inserts the given key into the dictionary, with a default constructed value.
        /// If the key already exists, the value is not changed and its address is returned.
        /// Aborts if the insert function is not set. The given dictionary must be a valid instance
        /// of this type, otherwise, undefined behavior occurs.
        /// @param dictionary Pointer to the dictionary.
        /// @param key Instance of the key type to insert.
        /// @return Address of the value.
        void* insert(void* dictionary, const void* key) const;

        /// @brief Inserts the given key into the dictionary, with a default constructed value.
        /// If the key already exists, the value is not changed and its address is returned.
        /// Aborts if the insert function is not set. The given dictionary must be a valid instance
        /// of this type and K must match the key type of the dictionary, otherwise, undefined
        /// behavior occurs.
        /// @tparam K Type of the keys.
        /// @tparam V Type of the values.
        /// @param dictionary Pointer to the dictionary.
        /// @param key Instance of the key type to insert.
        /// @return Address of the value.
        template <typename K, typename V>
        V& insert(void* dictionary, const K& key) const
        {
            return *static_cast<V*>(this->insert(dictionary, static_cast<const void*>(&key)));
        }

        /// @brief Removes the value with the given key from the dictionary.
        /// Aborts if the remove function is not set. The given dictionary must be a valid instance
        /// of this type, otherwise, undefined behavior occurs.
        /// @param dictionary Pointer to the dictionary.
        /// @param key Instance of the key type to remove.
        /// @return `true` if the key existed and was removed, `false` otherwise.
        bool remove(void* dictionary, const void* key) const;

        /// @brief Removes the value with the given key from the dictionary.
        /// Aborts if the remove function is not set. The given dictionary must be a valid instance
        /// of this type and K must match the key type of the dictionary, otherwise, undefined
        /// behavior occurs.
        /// @tparam K Type of the keys.
        /// @param dictionary Pointer to the dictionary.
        /// @param key Instance of the key type to remove.
        /// @return `true` if the key existed and was removed, `false` otherwise.
        template <typename K>
        bool remove(void* dictionary, const K& key) const
        {
            return this->remove(dictionary, static_cast<const void*>(&key));
        }

        /// @brief Clears the dictionary.
        /// Aborts if the clear function is not set. The given dictionary must be a valid instance
        /// of this type, otherwise, undefined behavior occurs.
        /// @param dictionary Pointer to the dictionary.
        void clear(void* dictionary) const;

        /// @brief Creates an iterator for the given dictionary.
        /// Aborts if hasIterator() returns `false`. The given dictionary must be a valid instance
        /// of this type, otherwise, undefined behavior occurs.
        /// @param dictionary Pointer to the dictionary.
        /// @return Iterator for the dictionary.
        Iterator iterator(const void* dictionary) const;

        virtual void accept(class TypeVisitor& visitor) const override;

    private:
        DictionaryType(std::string name, const Type& keyType, const Type& valueType);

        const Type& mKeyType;
        const Type& mValueType;
        Length mLength;
        Value mValue;
        Insert mInsert;
        Remove mRemove;
        Clear mClear;
        IteratorNew mIteratorNew;
        IteratorKey mIteratorKey;
        IteratorValue mIteratorValue;
        IteratorIncrement mIteratorIncrement;
        IteratorDelete mIteratorDelete;
    };
} // namespace cubos::core::reflection
