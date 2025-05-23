#pragma once

#include <concepts>

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

#include "../../utils.hpp"

using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

/// @brief Checks if a type's ConstructibleTrait has the correct size and alignment.
/// @tparam T Type.
template <typename T>
static void testConstructibleLayout()
{
    const Type& type = reflect<T>();
    REQUIRE(type.has<ConstructibleTrait>());
    auto& constructible = type.get<ConstructibleTrait>();

    CHECK(constructible.size() == sizeof(T));
    CHECK(constructible.alignment() == static_cast<std::size_t>(alignof(T)));
}

/// @brief Checks if a type's ConstructibleTrait default constructor works as expected.
/// @tparam T Type.
template <typename T>
static void testDefaultConstructible()
{
    const Type& type = reflect<T>();
    REQUIRE(type.has<ConstructibleTrait>());
    auto& constructible = type.get<ConstructibleTrait>();

    auto* instance = operator new(constructible.size());
    REQUIRE(constructible.hasDefaultConstruct());
    constructible.defaultConstruct(instance);

    if constexpr (std::equality_comparable<T> && !std::is_array_v<T>)
    {
        CHECK(*reinterpret_cast<T*>(instance) == T{});
    }

    constructible.destruct(instance);
    operator delete(instance);
}

/// @brief Checks if a type's ConstructibleTrait copy constructor works as expected.
/// @tparam T Type.
/// @param valueToCopy Value which will be copied.
template <typename T>
static void testCopyConstructible(const T& valueToCopy)
{
    const Type& type = reflect<T>();
    REQUIRE(type.has<ConstructibleTrait>());
    auto& constructible = type.get<ConstructibleTrait>();

    auto* instance = operator new(constructible.size());
    REQUIRE(constructible.hasCopyConstruct());
    constructible.copyConstruct(instance, &valueToCopy);

    if constexpr (std::equality_comparable<T>)
    {
        CHECK(*reinterpret_cast<T*>(instance) == valueToCopy);
    }

    constructible.destruct(instance);
    operator delete(instance);
}

/// @brief Checks if a type's ConstructibleTrait move constructor works as expected.
/// @tparam T Type.
/// @param valueToMove Value which will be moved.
template <typename T>
static void testMoveConstructible(T& valueToMove)
{
    const Type& type = reflect<T>();
    REQUIRE(type.has<ConstructibleTrait>());
    auto& constructible = type.get<ConstructibleTrait>();

    auto* instance = operator new(constructible.size());
    REQUIRE(constructible.hasMoveConstruct());
    constructible.moveConstruct(instance, &valueToMove);

    constructible.destruct(instance);
    operator delete(instance);
}

/// @brief Checks if a type's ConstructibleTrait has the expected information.
/// @tparam T Type.
/// @param valueToMove Value which will be moved - necessary if the type is copy or move
/// constructible.
template <typename T>
void testConstructible(T* valueToMove = nullptr)
{
    testConstructibleLayout<T>();

    if constexpr (std::default_initializable<T>)
    {
        testDefaultConstructible<T>();
    }

    if constexpr (std::copy_constructible<T>)
    {
        REQUIRE(valueToMove != nullptr);
        testCopyConstructible<T>(*valueToMove);
    }

    if constexpr (std::move_constructible<T>)
    {
        REQUIRE(valueToMove != nullptr);
        testMoveConstructible<T>(*valueToMove);
    }
}
