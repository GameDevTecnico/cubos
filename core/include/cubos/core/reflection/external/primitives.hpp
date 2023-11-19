/// @file
/// @brief Reflection declarations for primitive types.
/// @ingroup core-reflection

#pragma once

#include <cubos/core/reflection/reflect.hpp>

// Boolean type.
CUBOS_REFLECT_EXTERNAL_DECL(bool);

// Char type (distinct from `signed char` and `unsigned char`).
CUBOS_REFLECT_EXTERNAL_DECL(char);

// Signed integer types.
CUBOS_REFLECT_EXTERNAL_DECL(signed char);
CUBOS_REFLECT_EXTERNAL_DECL(short);
CUBOS_REFLECT_EXTERNAL_DECL(int);
CUBOS_REFLECT_EXTERNAL_DECL(long);
CUBOS_REFLECT_EXTERNAL_DECL(long long);

// Unsigned integer types.
CUBOS_REFLECT_EXTERNAL_DECL(unsigned char);
CUBOS_REFLECT_EXTERNAL_DECL(unsigned short);
CUBOS_REFLECT_EXTERNAL_DECL(unsigned int);
CUBOS_REFLECT_EXTERNAL_DECL(unsigned long);
CUBOS_REFLECT_EXTERNAL_DECL(unsigned long long);

// Floating point types.
CUBOS_REFLECT_EXTERNAL_DECL(float);
CUBOS_REFLECT_EXTERNAL_DECL(double);
