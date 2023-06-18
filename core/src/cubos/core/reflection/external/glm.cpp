#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cubos/core/reflection/array.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/object.hpp>

#define AUTO_VEC2(type)                                                                                                \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        return ObjectType::build(#type).field("x", &type::x).field("y", &type::y).defaultConstructible<type>().get();  \
    }

AUTO_VEC2(glm::ivec2);
AUTO_VEC2(glm::uvec2);
AUTO_VEC2(glm::vec2);

#define AUTO_VEC3(type)                                                                                                \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        return ObjectType::build(#type)                                                                                \
            .field("x", &type::x)                                                                                      \
            .field("y", &type::y)                                                                                      \
            .field("z", &type::z)                                                                                      \
            .defaultConstructible<type>()                                                                              \
            .get();                                                                                                    \
    }

AUTO_VEC3(glm::ivec3);
AUTO_VEC3(glm::uvec3);
AUTO_VEC3(glm::vec3);

#define AUTO_VEC4(type)                                                                                                \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        return ObjectType::build(#type)                                                                                \
            .field("x", &type::x)                                                                                      \
            .field("y", &type::y)                                                                                      \
            .field("z", &type::z)                                                                                      \
            .field("w", &type::w)                                                                                      \
            .defaultConstructible<type>()                                                                              \
            .get();                                                                                                    \
    }

AUTO_VEC4(glm::ivec4);
AUTO_VEC4(glm::uvec4);
AUTO_VEC4(glm::vec4);

#define ACCESS_MAT(type, x, y)                                                                                         \
    [](const void* mat) -> uintptr_t { return reinterpret_cast<uintptr_t>(&(*static_cast<const type*>(mat))[x][y]); }
#define AUTO_MAT_FIELD(type, x, y) field(#x #y, elementType, ACCESS_MAT(type, x, y))

#define AUTO_MAT2(type)                                                                                                \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        auto& elementType = reflect<type::value_type>();                                                               \
        return ObjectType::build(#type)                                                                                \
            .AUTO_MAT_FIELD(type, 0, 0)                                                                                \
            .AUTO_MAT_FIELD(type, 0, 1)                                                                                \
            .AUTO_MAT_FIELD(type, 1, 0)                                                                                \
            .AUTO_MAT_FIELD(type, 1, 1)                                                                                \
            .defaultConstructible<type>()                                                                              \
            .get();                                                                                                    \
    }

AUTO_MAT2(glm::mat2);

#define AUTO_MAT3(type)                                                                                                \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        auto& elementType = reflect<type::value_type>();                                                               \
        return ObjectType::build(#type)                                                                                \
            .AUTO_MAT_FIELD(type, 0, 0)                                                                                \
            .AUTO_MAT_FIELD(type, 0, 1)                                                                                \
            .AUTO_MAT_FIELD(type, 0, 2)                                                                                \
            .AUTO_MAT_FIELD(type, 1, 0)                                                                                \
            .AUTO_MAT_FIELD(type, 1, 1)                                                                                \
            .AUTO_MAT_FIELD(type, 1, 2)                                                                                \
            .AUTO_MAT_FIELD(type, 2, 0)                                                                                \
            .AUTO_MAT_FIELD(type, 2, 1)                                                                                \
            .AUTO_MAT_FIELD(type, 2, 2)                                                                                \
            .defaultConstructible<type>()                                                                              \
            .get();                                                                                                    \
    }

AUTO_MAT3(glm::mat3);

#define AUTO_MAT4(type)                                                                                                \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        auto& elementType = reflect<type::value_type>();                                                               \
        return ObjectType::build(#type)                                                                                \
            .AUTO_MAT_FIELD(type, 0, 0)                                                                                \
            .AUTO_MAT_FIELD(type, 0, 1)                                                                                \
            .AUTO_MAT_FIELD(type, 0, 2)                                                                                \
            .AUTO_MAT_FIELD(type, 0, 3)                                                                                \
            .AUTO_MAT_FIELD(type, 1, 0)                                                                                \
            .AUTO_MAT_FIELD(type, 1, 1)                                                                                \
            .AUTO_MAT_FIELD(type, 1, 2)                                                                                \
            .AUTO_MAT_FIELD(type, 1, 3)                                                                                \
            .AUTO_MAT_FIELD(type, 2, 0)                                                                                \
            .AUTO_MAT_FIELD(type, 2, 1)                                                                                \
            .AUTO_MAT_FIELD(type, 2, 2)                                                                                \
            .AUTO_MAT_FIELD(type, 2, 3)                                                                                \
            .AUTO_MAT_FIELD(type, 3, 0)                                                                                \
            .AUTO_MAT_FIELD(type, 3, 1)                                                                                \
            .AUTO_MAT_FIELD(type, 3, 2)                                                                                \
            .AUTO_MAT_FIELD(type, 3, 3)                                                                                \
            .defaultConstructible<type>()                                                                              \
            .get();                                                                                                    \
    }

AUTO_MAT4(glm::mat4);

// Reuse the vec4 macro for quaternions.
AUTO_VEC4(glm::quat);
