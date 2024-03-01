/// @file
/// @brief Reflection declarations for primitive types.
/// @ingroup core-reflection

#pragma once

#include <cubos/core/reflection/reflect.hpp>

// Boolean type.
CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, bool);

// Char type (distinct from `signed char` and `unsigned char`).
CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, char);

// Signed integer types.
CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, signed char);
CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, short);
CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, int);
CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, long);
CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, long long);

// Unsigned integer types.
CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, unsigned char);
CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, unsigned short);
CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, unsigned int);
CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, unsigned long);
CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, unsigned long long);

// Floating point types.
CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, float);
CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, double);
