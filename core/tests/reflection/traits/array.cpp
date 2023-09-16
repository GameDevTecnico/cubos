#include <doctest/doctest.h>

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/type.hpp>

#include "../../utils.hpp"

using cubos::core::reflection::ArrayTrait;
using cubos::core::reflection::reflect;

TEST_CASE("reflection::ArrayTrait")
{
    auto trait = ArrayTrait(
        reflect<int>(), [](const void*) -> std::size_t { return 2; },
        [](const void* array, std::size_t index) { return reinterpret_cast<uintptr_t>(array) + sizeof(int) * index; });

    SUBCASE("without any resizing operations")
    {
        int instance[2];
        CHECK(trait.elementType().is<int>());
        CHECK(trait.length(&instance) == 2);
        CHECK(trait.get(&instance, 0) == &instance[0]);
        CHECK(trait.get(static_cast<const void*>(&instance), 1) == &instance[1]);

        // No insert or erase operations were provided
        CHECK_FALSE(trait.hasInsertDefault());
        CHECK_FALSE(trait.hasInsertCopy());
        CHECK_FALSE(trait.hasInsertMove());
        CHECK_FALSE(trait.hasErase());
        CHECK_FALSE(trait.insertDefault(&instance, 0));
        CHECK_FALSE(trait.insertCopy(&instance, 0, &instance[1]));
        CHECK_FALSE(trait.insertMove(&instance, 0, &instance[1]));
        CHECK_FALSE(trait.erase(&instance, 0));
    }

    // These subcases below are not really inserting or erasing anything, but they are testing that
    // the insert and erase methods are called correctly.

    SUBCASE("with default-constructing inserts")
    {
        trait.insertDefault([](void* array, std::size_t index) { static_cast<int*>(array)[index] = 42; });

        int instance[2] = {1, 2};
        CHECK(trait.hasInsertDefault());
        CHECK(trait.insertDefault(&instance, 0));
        CHECK(instance[0] == 42);
        CHECK(instance[1] == 2);
    }

    SUBCASE("with copy-constructing inserts")
    {
        trait.insertCopy([](void* array, std::size_t index, const void* value) {
            static_cast<int*>(array)[index] = *static_cast<const int*>(value);
        });

        int instance[2] = {1, 2};
        CHECK(trait.hasInsertCopy());
        CHECK(trait.insertCopy(&instance, 0, &instance[1]));
        CHECK(instance[0] == 2);
        CHECK(instance[1] == 2);
    }

    SUBCASE("with move-constructing inserts")
    {
        trait.insertMove([](void* array, std::size_t index, void* value) {
            static_cast<int*>(array)[index] = *static_cast<int*>(value);
        });

        int instance[2] = {1, 2};
        CHECK(trait.hasInsertMove());
        CHECK(trait.insertMove(&instance, 1, &instance[0]));
        CHECK(instance[0] == 1);
        CHECK(instance[1] == 1);
    }

    SUBCASE("with erases")
    {
        trait.erase([](void* array, std::size_t index) { static_cast<int*>(array)[index] = 0; });

        int instance[2] = {1, 2};
        CHECK(trait.hasErase());
        CHECK(trait.erase(&instance, 1));
        CHECK(instance[0] == 1);
        CHECK(instance[1] == 0);
    }
}
