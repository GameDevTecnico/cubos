#pragma once

#include <string>

// Only include the type header, which contains the reflection forward declaration.
#include <cubos/core/reflection/reflect.hpp>

// A simple struct to be reflected.
struct Fruit
{
    std::string name;
    int weight;

    // Reflection forward declaration - without it, `reflect<Fruit>()` would not compile.
    // Must be have a corresponding CUBOS_REFLECT_IMPL somewhere - usually in a source file.
    CUBOS_REFLECT;
};
