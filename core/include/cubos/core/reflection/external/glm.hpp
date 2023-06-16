/// @file
/// @brief Declares common glm types as reflectable.

#pragma once

#include <glm/fwd.hpp>

#include <cubos/core/reflection/reflect.hpp>

/// @name Declarations of reflect() for common glm types.
/// @{
CUBOS_REFLECT_EXTERNAL_DECL(glm::ivec2);
CUBOS_REFLECT_EXTERNAL_DECL(glm::uvec2);
CUBOS_REFLECT_EXTERNAL_DECL(glm::vec2);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dvec2);

CUBOS_REFLECT_EXTERNAL_DECL(glm::ivec3);
CUBOS_REFLECT_EXTERNAL_DECL(glm::uvec3);
CUBOS_REFLECT_EXTERNAL_DECL(glm::vec3);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dvec3);

CUBOS_REFLECT_EXTERNAL_DECL(glm::ivec4);
CUBOS_REFLECT_EXTERNAL_DECL(glm::uvec4);
CUBOS_REFLECT_EXTERNAL_DECL(glm::vec4);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dvec4);

CUBOS_REFLECT_EXTERNAL_DECL(glm::mat2);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dmat2);

CUBOS_REFLECT_EXTERNAL_DECL(glm::mat3);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dmat3);

CUBOS_REFLECT_EXTERNAL_DECL(glm::mat4);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dmat4);

CUBOS_REFLECT_EXTERNAL_DECL(glm::quat);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dquat);
/// @}
