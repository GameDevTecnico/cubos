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

    REQUIRE(trait.view(&value).length() == length);
    REQUIRE(trait.elementType().is<E>());

    std::size_t i = 0;
    for (auto* el : trait.view(&value))
    {
        CHECK(el == trait.view(&value).get(i));
        i += 1;
    }

    CHECK(i == length);
    i = 0;
    for (auto* el : trait.view(static_cast<const void*>(&value)))
    {
        CHECK(el == trait.view(static_cast<const void*>(&value)).get(i));
        i += 1;
    }

    if (trait.hasInsertDefault())
    {
        trait.view(&value).insertDefault(length);
        CHECK(trait.view(&value).length() == length + 1);
        trait.view(&value).insertDefault(0);
        CHECK(trait.view(static_cast<const void*>(&value)).length() == length + 2);
        length += 2;

        if constexpr (std::equality_comparable<E> && std::default_initializable<E>)
        {
            CHECK(*reinterpret_cast<E*>(trait.view(&value).get(0)) == E{});
            CHECK(*reinterpret_cast<const E*>(trait.view(static_cast<const void*>(&value)).get(length - 1)) == E{});
        }
    }

    if (trait.hasErase())
    {
        for (; length > 0; --length)
        {
            trait.view(&value).erase(length - 1);
            CHECK(trait.view(&value).length() == length - 1);
        }
    }

    if (inserted)
    {
        if (trait.hasInsertCopy())
        {
            trait.view(&value).insertCopy(0, inserted);
            CHECK(trait.view(&value).length() == length + 1);
            length += 1;

            if constexpr (std::equality_comparable<E>)
            {
                CHECK(*reinterpret_cast<E*>(trait.view(&value).get(0)) == *inserted);
            }
        }

        if (trait.hasInsertMove())
        {
            trait.view(&value).insertMove((length + 1) / 2, inserted);
            CHECK(trait.view(&value).length() == length + 1);
            length += 1;
        }
    }
}
