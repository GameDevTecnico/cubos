#pragma once

#include <doctest/doctest.h>

#include <cubos/core/reflection/reflect.hpp>

/// @brief Tests the base Type getters of the given type.
/// @tparam T Type to test.
template <typename T, typename Kind>
static void testTypeGetters(const char* name, const char* shortName)
{
    auto& t = cubos::core::reflection::reflect<T>();
    CHECK(t.template isKind<Kind>());
    CHECK(t.template is<T>());
    CHECK(t.name() == name);
    CHECK(t.shortName() == shortName);
}

/// @brief Tests if the given type default constructor works as expected.
/// @tparam T Type to test.
template <typename T>
static void testTypeDefaultConstructor()
{
    auto& t = cubos::core::reflection::reflect<T>();

    // Check if the default constructor really creates a default instance of the type.
    auto* instance = operator new (t.size(), std::align_val_t{t.alignment()});
    t.defaultConstruct(instance);

    CHECK(*static_cast<T*>(instance) == T{});
    t.destroy(instance);
    operator delete(instance);
}

/// @brief Tests if the given type move constructor works as expected.
/// @tparam T Type to test.
template <typename T>
static void testTypeMoveConstructor()
{
    auto& t = cubos::core::reflection::reflect<T>();

    // Check if the move constructor really moves the given instance.
    auto moved = T{};
    auto* instance = operator new (t.size(), std::align_val_t{t.alignment()});
    t.moveConstruct(instance, &moved);

    CHECK(*static_cast<T*>(instance) == moved);
    t.destroy(instance);
    operator delete(instance);
}
