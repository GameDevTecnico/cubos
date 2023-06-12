/// @file
/// @brief Defines the DictionaryType class, which is an implementation of Type for dictionary types.

#pragma once

#include <unordered_map>

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

            /// @name Getters for the value which the iterator is currently pointing to.
            /// @brief Gets the value which the iterator is currently pointing to.
            /// @param dictionary Instance of the dictionary.
            /// @return Value which the iterator is currently pointing to.
            /// @{
            const void* value(const void* dictionary) const;
            void* value(void* dictionary) const;
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
        /// @return Address of the value, or `nullptr` if the key does not exist.
        /// @{
        const void* value(const void* dictionary, const void* key) const;
        void* value(void* dictionary, const void* key) const;
        /// @}

        /// @brief Inserts the given key into the dictionary, with a default constructed value.
        /// If the key already exists, the value is not changed and its address is returned.
        /// Aborts if the insert function is not set. The given dictionary must be a valid instance
        /// of this type, otherwise, undefined behavior occurs.
        /// @param dictionary Pointer to the dictionary.
        /// @param key Instance of the key type to insert.
        /// @return Address of the value.
        void* insert(void* dictionary, const void* key) const;

        /// @brief Removes the value with the given key from the dictionary.
        /// Aborts if the remove function is not set. The given dictionary must be a valid instance
        /// of this type, otherwise, undefined behavior occurs.
        /// @param dictionary Pointer to the dictionary.
        /// @param key Instance of the key type to remove.
        /// @return `true` if the key existed and was removed, `false` otherwise.
        bool remove(void* dictionary, const void* key) const;

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
        IteratorNew mIteratorNew;
        IteratorKey mIteratorKey;
        IteratorValue mIteratorValue;
        IteratorIncrement mIteratorIncrement;
        IteratorDelete mIteratorDelete;
    };
} // namespace cubos::core::reflection

/// @name Define reflection functions for std::unordered_map.
/// @{
CUBOS_REFLECT_EXTERNAL_TEMPLATE((typename K, typename V), (std::unordered_map<K, V>))
{
    using Iterator = typename std::unordered_map<K, V>::iterator;

    std::string name = "std::unordered_map<" + reflect<K>().name() + ", " + reflect<V>().name() + ">";
    return DictionaryType::build(std::move(name), reflect<K>(), reflect<V>())
        .length([](const void* dictionary) { return static_cast<const std::unordered_map<K, V>*>(dictionary)->size(); })
        .value([](const void* dictionary, const void* key) {
            auto& map = *static_cast<const std::unordered_map<K, V>*>(dictionary);
            auto it = map.find(*static_cast<const K*>(key));
            return it == map.end() ? 0 : reinterpret_cast<uintptr_t>(&it->second);
        })
        .insert([](void* dictionary, const void* key) -> void* {
            auto& map = *static_cast<std::unordered_map<K, V>*>(dictionary);
            auto it = map.find(*static_cast<const K*>(key));
            if (it == map.end())
            {
                return &map.emplace(*static_cast<const K*>(key), V{}).first->second;
            }
            else
            {
                return &it->second;
            }
        })
        .remove([](void* dictionary, const void* key) {
            auto& map = *static_cast<std::unordered_map<K, V>*>(dictionary);
            return map.erase(*static_cast<const K*>(key)) == 1;
        })
        .iteratorNew([](const void* dictionary) -> void* {
            auto& map = *static_cast<std::unordered_map<K, V>*>(const_cast<void*>(dictionary));
            if (map.empty())
            {
                return nullptr;
            }

            return new Iterator(map.begin());
        })
        .iteratorKey([](const void*, const void* iterator) -> const void* {
            auto& it = *static_cast<const Iterator*>(iterator);
            return &it->first;
        })
        .iteratorValue([](const void*, const void* iterator) {
            auto& it = *static_cast<const Iterator*>(iterator);
            return reinterpret_cast<uintptr_t>(&it->second);
        })
        .iteratorIncrement([](const void* dictionary, void* iterator) {
            auto& map = *static_cast<std::unordered_map<K, V>*>(const_cast<void*>(dictionary));
            auto& it = *static_cast<Iterator*>(iterator);
            ++it;

            if (it == map.end())
            {
                delete &it;
                return true;
            }

            return false;
        })
        .iteratorDelete([](void* iterator) { delete static_cast<Iterator*>(iterator); })
        .get();
}
#undef COMMA
/// @}
