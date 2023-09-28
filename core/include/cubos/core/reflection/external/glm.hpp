/// @file
/// @brief Reflection declarations for external glm types.
/// @ingroup core-reflection

#pragma once

#include <glm/ext/quaternion_double.hpp> // dquat
#include <glm/ext/quaternion_float.hpp>  // quat
#include <glm/mat2x2.hpp>                // mat2, dmat2
#include <glm/mat2x3.hpp>                // mat2x3, dmat2x3
#include <glm/mat2x4.hpp>                // mat2x4, dmat2x4
#include <glm/mat3x2.hpp>                // mat3x2, dmat3x2
#include <glm/mat3x3.hpp>                // mat3, dmat3
#include <glm/mat3x4.hpp>                // mat3x4, dmat3x4
#include <glm/mat4x2.hpp>                // mat4x2, dmat4x2
#include <glm/mat4x3.hpp>                // mat4x3, dmat4x3
#include <glm/mat4x4.hpp>                // mat4, dmat4
#include <glm/vec2.hpp>                  // vec2, bvec2, dvec2, ivec2 and uvec2
#include <glm/vec3.hpp>                  // vec3, bvec3, dvec3, ivec3 and uvec3
#include <glm/vec4.hpp>                  // vec4, bvec4, dvec4, ivec4 and uvec4

#include <cubos/core/reflection/reflect.hpp>

CUBOS_REFLECT_EXTERNAL_DECL(glm::quat);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dquat);
CUBOS_REFLECT_EXTERNAL_DECL(glm::mat2);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dmat2);
CUBOS_REFLECT_EXTERNAL_DECL(glm::mat2x3);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dmat2x3);
CUBOS_REFLECT_EXTERNAL_DECL(glm::mat2x4);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dmat2x4);
CUBOS_REFLECT_EXTERNAL_DECL(glm::mat3x2);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dmat3x2);
CUBOS_REFLECT_EXTERNAL_DECL(glm::mat3);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dmat3);
CUBOS_REFLECT_EXTERNAL_DECL(glm::mat3x4);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dmat3x4);
CUBOS_REFLECT_EXTERNAL_DECL(glm::mat4x2);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dmat4x2);
CUBOS_REFLECT_EXTERNAL_DECL(glm::mat4x3);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dmat4x3);
CUBOS_REFLECT_EXTERNAL_DECL(glm::mat4);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dmat4);
CUBOS_REFLECT_EXTERNAL_DECL(glm::vec2);
CUBOS_REFLECT_EXTERNAL_DECL(glm::bvec2);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dvec2);
CUBOS_REFLECT_EXTERNAL_DECL(glm::ivec2);
CUBOS_REFLECT_EXTERNAL_DECL(glm::uvec2);
CUBOS_REFLECT_EXTERNAL_DECL(glm::vec3);
CUBOS_REFLECT_EXTERNAL_DECL(glm::bvec3);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dvec3);
CUBOS_REFLECT_EXTERNAL_DECL(glm::ivec3);
CUBOS_REFLECT_EXTERNAL_DECL(glm::uvec3);
CUBOS_REFLECT_EXTERNAL_DECL(glm::vec4);
CUBOS_REFLECT_EXTERNAL_DECL(glm::dvec4);
CUBOS_REFLECT_EXTERNAL_DECL(glm::bvec4);
CUBOS_REFLECT_EXTERNAL_DECL(glm::ivec4);
CUBOS_REFLECT_EXTERNAL_DECL(glm::uvec4);
