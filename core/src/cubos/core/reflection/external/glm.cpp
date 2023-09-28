#include <glm/gtc/type_ptr.hpp>

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

#define AUTO_IMPL(type, name, inner_type, size)                                                                        \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        return Type::create(name)                                                                                      \
            .with(ConstructibleTrait::typed<type>()                                                                    \
                      .withDefaultConstructor()                                                                        \
                      .withCopyConstructor()                                                                           \
                      .withMoveConstructor()                                                                           \
                      .build())                                                                                        \
            .with(ArrayTrait(                                                                                          \
                reflect<inner_type>(), [](const void* /*unused*/) -> std::size_t { return size; },                     \
                [](const void* instance, std::size_t index) -> uintptr_t {                                             \
                    return reinterpret_cast<uintptr_t>(glm::value_ptr(*(static_cast<const type*>(instance))) + index); \
                }));                                                                                                   \
    }

AUTO_IMPL(glm::quat, "glm::quat", float, 4);
AUTO_IMPL(glm::dquat, "glm::dquat", double, 4);
AUTO_IMPL(glm::mat2, "glm::mat2", float, 2 * 2);
AUTO_IMPL(glm::dmat2, "glm::dmat2", double, 2 * 2);
AUTO_IMPL(glm::mat2x3, "glm::mat2x3", float, 2 * 3);
AUTO_IMPL(glm::dmat2x3, "glm::dmat2x3", double, 2 * 3);
AUTO_IMPL(glm::mat2x4, "glm::mat2x4", float, 2 * 4);
AUTO_IMPL(glm::dmat2x4, "glm::dmat2x4", double, 2 * 4);
AUTO_IMPL(glm::mat3x2, "glm::mat3x2", float, 3 * 2);
AUTO_IMPL(glm::dmat3x2, "glm::dmat3x2", double, 3 * 2);
AUTO_IMPL(glm::mat3, "glm::mat3", float, 3 * 3);
AUTO_IMPL(glm::dmat3, "glm::dmat3", double, 3 * 3);
AUTO_IMPL(glm::mat3x4, "glm::mat3x4", float, 3 * 4);
AUTO_IMPL(glm::dmat3x4, "glm::dmat3x4", double, 3 * 4);
AUTO_IMPL(glm::mat4x2, "glm::mat4x2", float, 4 * 2);
AUTO_IMPL(glm::dmat4x2, "glm::dmat4x2", double, 4 * 2);
AUTO_IMPL(glm::mat4x3, "glm::mat4x3", float, 4 * 3);
AUTO_IMPL(glm::dmat4x3, "glm::dmat4x3", double, 4 * 3);
AUTO_IMPL(glm::mat4, "glm::mat4", float, 4 * 4);
AUTO_IMPL(glm::dmat4, "glm::dmat4", double, 4 * 4);
AUTO_IMPL(glm::vec2, "glm::vec2", float, 2);
AUTO_IMPL(glm::bvec2, "glm::bvec2", bool, 2);
AUTO_IMPL(glm::dvec2, "glm::dvec2", double, 2);
AUTO_IMPL(glm::ivec2, "glm::ivec2", int32_t, 2);
AUTO_IMPL(glm::uvec2, "glm::uvec2", uint32_t, 2);
AUTO_IMPL(glm::vec3, "glm::vec3", float, 3);
AUTO_IMPL(glm::bvec3, "glm::bvec3", bool, 3);
AUTO_IMPL(glm::dvec3, "glm::dvec3", double, 3);
AUTO_IMPL(glm::ivec3, "glm::ivec3", int32_t, 3);
AUTO_IMPL(glm::uvec3, "glm::uvec3", uint32_t, 3);
AUTO_IMPL(glm::vec4, "glm::vec4", float, 4);
AUTO_IMPL(glm::dvec4, "glm::dvec4", double, 4);
AUTO_IMPL(glm::bvec4, "glm::bvec4", bool, 4);
AUTO_IMPL(glm::ivec4, "glm::ivec4", int32_t, 4);
AUTO_IMPL(glm::uvec4, "glm::uvec4", uint32_t, 4);
