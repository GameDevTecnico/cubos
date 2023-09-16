#include <concepts>

#include <doctest/doctest.h>

#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::ArrayTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

/// @brief Checks if a type's ArrayTrait works as expected.
/// @tparam T Type.
/// @tparam E Element type.
/// @param value Value to test.
/// @param length Initial length.
/// @param inserted Optional value to insert.
template <typename T, typename E>
void testArray(T& value, std::size_t length, E* inserted = nullptr)
{
    const Type& type = reflect<T>();
    REQUIRE(type.has<ArrayTrait>());
    const ArrayTrait& trait = type.get<ArrayTrait>();

    REQUIRE(trait.length(&value) == length);
    REQUIRE(trait.elementType().is<E>());

    if (trait.hasInsertDefault())
    {
        trait.insertDefault(&value, length);
        CHECK(trait.length(&value) == length + 1);
        trait.insertDefault(&value, 0);
        CHECK(trait.length(&value) == length + 2);
        length += 2;

        if constexpr (std::equality_comparable<E> && std::default_initializable<E>)
        {
            CHECK(*reinterpret_cast<E*>(trait.get(&value, 0)) == E{});
            CHECK(*reinterpret_cast<const E*>(trait.get(static_cast<const void*>(&value), length - 1)) == E{});
        }
    }

    if (trait.hasErase())
    {
        for (; length > 0; --length)
        {
            trait.erase(&value, length - 1);
            CHECK(trait.length(&value) == length - 1);
        }
    }

    if (inserted)
    {
        if (trait.hasInsertCopy())
        {
            trait.insertCopy(&value, 0, inserted);
            CHECK(trait.length(&value) == length + 1);
            length += 1;

            if constexpr (std::equality_comparable<E>)
            {
                CHECK(*reinterpret_cast<E*>(trait.get(&value, 0)) == *inserted);
            }
        }

        if (trait.hasInsertMove())
        {
            trait.insertMove(&value, (length + 1) / 2, inserted);
            CHECK(trait.length(&value) == length + 1);
            length += 1;
        }
    }
}
