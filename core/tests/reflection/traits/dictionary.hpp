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

    REQUIRE(trait.keyType().is<K>());
    REQUIRE(trait.valueType().is<V>());

    auto view = trait.view(&value);
    auto constView = trait.view(static_cast<const void*>(&value));
    REQUIRE(view.length() == length);
    REQUIRE(constView.length() == length);

    // Check that both the normal and const iterators count the expected number of elements.

    std::size_t i = 0;
    for (auto entry : view)
    {
        (void)entry;
        i += 1;
    }
    REQUIRE(i == length);

    i = 0;
    for (auto entry : constView)
    {
        (void)entry;
        i += 1;
    }
    REQUIRE(i == length);

    if (trait.hasInsertDefault())
    {

        REQUIRE(constView.find(insertedKey) == constView.end());
        view.insertDefault(insertedKey);
        auto it = view.find(insertedKey);
        REQUIRE(it != view.end());

        if constexpr (std::equality_comparable<K>)
        {
            CHECK(*static_cast<const K*>(it->key) == *insertedKey);
        }

        if constexpr (std::equality_comparable<V> && std::default_initializable<V>)
        {
            CHECK(*static_cast<const V*>(it->value) == V{});
        }

        REQUIRE(view.length() == length + 1);
        view.erase(it);
        REQUIRE(view.length() == length);
        REQUIRE(view.find(insertedKey) == view.end());
    }

    if (trait.hasInsertCopy())
    {
        REQUIRE(view.find(insertedKey) == view.end());
        view.insertCopy(insertedKey, insertedValue);
        auto it = view.find(insertedKey);
        REQUIRE(it != view.end());

        if constexpr (std::equality_comparable<K>)
        {
            CHECK(*static_cast<const K*>(it->key) == *insertedKey);
        }

        if constexpr (std::equality_comparable<V>)
        {
            CHECK(*static_cast<const V*>(it->value) == *static_cast<const V*>(insertedValue));
        }

        REQUIRE(view.length() == length + 1);
        view.erase(it);
        REQUIRE(view.length() == length);
        REQUIRE(view.find(insertedKey) == view.end());
    }

    if (trait.hasInsertMove())
    {
        REQUIRE(view.find(insertedKey) == view.end());
        view.insertMove(insertedKey, insertedValue);
        auto it = view.find(insertedKey);
        REQUIRE(it != view.end());

        if constexpr (std::equality_comparable<K>)
        {
            CHECK(*static_cast<const K*>(it->key) == *insertedKey);
        }

        REQUIRE(view.length() == length + 1);
        view.erase(it);
        REQUIRE(view.length() == length);
        REQUIRE(view.find(insertedKey) == view.end());
    }
}
