#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

#define AUTO_IMPL(type, name)                                                                                          \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        return Type::create(name).with(ConstructibleTrait::typed<type>()                                               \
                                           .withDefaultConstructor()                                                   \
                                           .withCopyConstructor()                                                      \
                                           .withMoveConstructor()                                                      \
                                           .build());                                                                  \
    }

AUTO_IMPL(glm::mat2, "glm::mat2");
AUTO_IMPL(glm::dmat2, "glm::dmat2");
AUTO_IMPL(glm::mat2x3, "glm::mat2x3");
AUTO_IMPL(glm::dmat2x3, "glm::dmat2x3");
AUTO_IMPL(glm::mat2x4, "glm::mat2x4");
AUTO_IMPL(glm::dmat2x4, "glm::dmat2x4");
AUTO_IMPL(glm::mat3x2, "glm::mat3x2");
AUTO_IMPL(glm::dmat3x2, "glm::dmat3x2");
AUTO_IMPL(glm::mat3, "glm::mat3");
AUTO_IMPL(glm::dmat3, "glm::dmat3");
AUTO_IMPL(glm::mat3x4, "glm::mat3x4");
AUTO_IMPL(glm::dmat3x4, "glm::dmat3x4");
AUTO_IMPL(glm::mat4x2, "glm::mat4x2");
AUTO_IMPL(glm::dmat4x2, "glm::dmat4x2");
AUTO_IMPL(glm::mat4x3, "glm::mat4x3");
AUTO_IMPL(glm::dmat4x3, "glm::dmat4x3");
AUTO_IMPL(glm::mat4, "glm::mat4");
AUTO_IMPL(glm::dmat4, "glm::dmat4");
AUTO_IMPL(glm::vec2, "glm::vec2");
AUTO_IMPL(glm::bvec2, "glm::bvec2");
AUTO_IMPL(glm::dvec2, "glm::dvec2");
AUTO_IMPL(glm::ivec2, "glm::ivec2");
AUTO_IMPL(glm::uvec2, "glm::uvec2");
AUTO_IMPL(glm::vec3, "glm::vec3");
AUTO_IMPL(glm::bvec3, "glm::bvec3");
AUTO_IMPL(glm::dvec3, "glm::dvec3");
AUTO_IMPL(glm::ivec3, "glm::ivec3");
AUTO_IMPL(glm::uvec3, "glm::uvec3");
AUTO_IMPL(glm::vec4, "glm::vec4");
AUTO_IMPL(glm::dvec4, "glm::dvec4");
AUTO_IMPL(glm::bvec4, "glm::bvec4");
AUTO_IMPL(glm::ivec4, "glm::ivec4");
AUTO_IMPL(glm::uvec4, "glm::uvec4");
