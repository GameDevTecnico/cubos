/// @file
/// @brief Declares primitive types such as `int` and `float` as reflectable.

#pragma once

#include <cstdint>

#include <cubos/core/reflection/reflect.hpp>

/// @name Declarations of reflect() for common primitive types.
/// @{
CUBOS_REFLECT_EXTERNAL_DECL(bool);
CUBOS_REFLECT_EXTERNAL_DECL(char);
CUBOS_REFLECT_EXTERNAL_DECL(int8_t);
CUBOS_REFLECT_EXTERNAL_DECL(int16_t);
CUBOS_REFLECT_EXTERNAL_DECL(int32_t);
CUBOS_REFLECT_EXTERNAL_DECL(int64_t);
CUBOS_REFLECT_EXTERNAL_DECL(uint8_t);
CUBOS_REFLECT_EXTERNAL_DECL(uint16_t);
CUBOS_REFLECT_EXTERNAL_DECL(uint32_t);
CUBOS_REFLECT_EXTERNAL_DECL(uint64_t);
CUBOS_REFLECT_EXTERNAL_DECL(float);
CUBOS_REFLECT_EXTERNAL_DECL(double);
/// @}
