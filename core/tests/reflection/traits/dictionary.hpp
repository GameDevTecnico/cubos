#include <concepts>

#include <doctest/doctest.h>

#include <cubos/core/reflection/traits/dictionary.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::DictionaryTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

/// @brief Checks if a type's DictionaryTrait works as expected.
/// @tparam T Type.
/// @tparam E Element type.
/// @param value Value to test.
/// @param length Initial length.
/// @param inserted Optional value to insert.
template <typename T, typename K, typename V>
void testDictionary(T& value, std::size_t length, const K* insertedKey, V* insertedValue)
{
    (void)insertedValue;

    const Type& type = reflect<T>();
    REQUIRE(type.has<DictionaryTrait>());
    const DictionaryTrait& trait = type.get<DictionaryTrait>();

    REQUIRE(trait.length(&value) == length);
    REQUIRE(trait.keyType().is<K>());
    REQUIRE(trait.valueType().is<V>());

    // Check that both the normal and const iterators count the expected number of elements.

    std::size_t i = 0;
    for (auto it = trait.begin(&value); !it.isNull(); it.advance())
    {
        i += 1;
    }
    REQUIRE(i == length);

    i = 0;
    for (auto it = trait.begin(static_cast<const void*>(&value)); !it.isNull(); it.advance())
    {
        i += 1;
    }
    REQUIRE(i == length);

    if (trait.hasInsertDefault())
    {
        REQUIRE(trait.find(&value, insertedKey).isNull());
        REQUIRE(trait.insertDefault(&value, insertedKey));
        auto it = trait.find(&value, insertedKey);
        REQUIRE(!it.isNull());

        if constexpr (std::equality_comparable<K>)
        {
            CHECK(*static_cast<const K*>(it.key()) == *insertedKey);
        }

        if constexpr (std::equality_comparable<V> && std::default_initializable<V>)
        {
            CHECK(*static_cast<const V*>(it.value()) == V{});
        }

        REQUIRE(trait.length(&value) == length + 1);
        REQUIRE(trait.erase(&value, it));
        REQUIRE(trait.length(&value) == length);
        REQUIRE(trait.find(&value, insertedKey).isNull());
    }

    if (trait.hasInsertCopy())
    {
        REQUIRE(trait.find(&value, insertedKey).isNull());
        REQUIRE(trait.insertCopy(&value, insertedKey, insertedValue));
        auto it = trait.find(&value, insertedKey);
        REQUIRE(!it.isNull());

        if constexpr (std::equality_comparable<K>)
        {
            CHECK(*static_cast<const K*>(it.key()) == *insertedKey);
        }

        if constexpr (std::equality_comparable<V>)
        {
            CHECK(*static_cast<const V*>(it.value()) == *static_cast<const V*>(insertedValue));
        }

        REQUIRE(trait.length(&value) == length + 1);
        REQUIRE(trait.erase(&value, it));
        REQUIRE(trait.length(&value) == length);
        REQUIRE(trait.find(&value, insertedKey).isNull());
    }

    if (trait.hasInsertMove())
    {
        REQUIRE(trait.find(&value, insertedKey).isNull());
        REQUIRE(trait.insertMove(&value, insertedKey, insertedValue));
        auto it = trait.find(&value, insertedKey);
        REQUIRE(!it.isNull());

        if constexpr (std::equality_comparable<K>)
        {
            CHECK(*static_cast<const K*>(it.key()) == *insertedKey);
        }

        REQUIRE(trait.length(&value) == length + 1);
        REQUIRE(trait.erase(&value, it));
        REQUIRE(trait.length(&value) == length);
        REQUIRE(trait.find(&value, insertedKey).isNull());
    }
}
