#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::FieldsTrait;

template <typename T>
class AddressOfImpl final : public FieldsTrait::AddressOf
{
public:
    /// @brief Constructs.
    /// @param column Column index.
    AddressOfImpl(size_t column)
        : mColumn(column)
    {
    }

    uintptr_t get(const void* instance) const override
    {
        return reinterpret_cast<uintptr_t>(&(static_cast<const T*>(instance)[mColumn]));
    }

private:
    size_t mColumn;
};

#define AUTO_CONSTRUCTIBLE(type)                                                                                       \
    ConstructibleTrait::typed<type>().withDefaultConstructor().withCopyConstructor().withMoveConstructor().build()

#define AUTO_VEC2(type) FieldsTrait().withField("0", &type::x).withField("1", &type::y)

#define AUTO_VEC3(type) AUTO_VEC2(type).withField("2", &type::z)

#define AUTO_VEC4(type) AUTO_VEC3(type).withField("3", &type::w)

#define AUTO_MAT2(type, column_type)                                                                                   \
    FieldsTrait()                                                                                                      \
        .withField(reflect<column_type>(), "0", new AddressOfImpl<type>(0))                                            \
        .withField(reflect<column_type>(), "1", new AddressOfImpl<type>(1))

#define AUTO_MAT3(type, column_type)                                                                                   \
    AUTO_MAT2(type, column_type).withField(reflect<column_type>(), "2", new AddressOfImpl<type>(2))

#define AUTO_MAT4(type, column_type)                                                                                   \
    AUTO_MAT3(type, column_type).withField(reflect<column_type>(), "3", new AddressOfImpl<type>(3))

#define AUTO_IMPL_VEC4(type, name)                                                                                     \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        return Type::create(name).with(AUTO_CONSTRUCTIBLE(type)).with(AUTO_VEC4(type));                                \
    }

#define AUTO_IMPL_VEC3(type, name)                                                                                     \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        return Type::create(name).with(AUTO_CONSTRUCTIBLE(type)).with(AUTO_VEC3(type));                                \
    }

#define AUTO_IMPL_VEC2(type, name)                                                                                     \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        return Type::create(name).with(AUTO_CONSTRUCTIBLE(type)).with(AUTO_VEC2(type));                                \
    }

#define AUTO_IMPL_MAT2(type, name, column_type)                                                                        \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        return Type::create(name).with(AUTO_CONSTRUCTIBLE(type)).with(AUTO_MAT2(type, column_type));                   \
    }

#define AUTO_IMPL_MAT3(type, name, column_type)                                                                        \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        return Type::create(name).with(AUTO_CONSTRUCTIBLE(type)).with(AUTO_MAT3(type, column_type));                   \
    }

#define AUTO_IMPL_MAT4(type, name, column_type)                                                                        \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        return Type::create(name).with(AUTO_CONSTRUCTIBLE(type)).with(AUTO_MAT4(type, column_type));                   \
    }

AUTO_IMPL_MAT2(glm::mat2, "glm::mat2", glm::vec2);
AUTO_IMPL_MAT2(glm::dmat2, "glm::dmat2", glm::dvec2);
AUTO_IMPL_MAT2(glm::mat2x3, "glm::mat2x3", glm::vec3);
AUTO_IMPL_MAT2(glm::dmat2x3, "glm::dmat2x3", glm::dvec3);
AUTO_IMPL_MAT2(glm::mat2x4, "glm::mat2x4", glm::vec4);
AUTO_IMPL_MAT2(glm::dmat2x4, "glm::dmat2x4", glm::dvec4);
AUTO_IMPL_MAT3(glm::mat3x2, "glm::mat3x2", glm::vec2);
AUTO_IMPL_MAT3(glm::dmat3x2, "glm::dmat3x2", glm::dvec2);
AUTO_IMPL_MAT3(glm::mat3, "glm::mat3", glm::vec3);
AUTO_IMPL_MAT3(glm::dmat3, "glm::dmat3", glm::dvec3);
AUTO_IMPL_MAT3(glm::mat3x4, "glm::mat3x4", glm::vec4);
AUTO_IMPL_MAT3(glm::dmat3x4, "glm::dmat3x4", glm::dvec4);
AUTO_IMPL_MAT4(glm::mat4x2, "glm::mat4x2", glm::vec2);
AUTO_IMPL_MAT4(glm::dmat4x2, "glm::dmat4x2", glm::dvec2);
AUTO_IMPL_MAT4(glm::mat4x3, "glm::mat4x3", glm::vec3);
AUTO_IMPL_MAT4(glm::dmat4x3, "glm::dmat4x3", glm::dvec3);
AUTO_IMPL_MAT4(glm::mat4, "glm::mat4", glm::vec4);
AUTO_IMPL_MAT4(glm::dmat4, "glm::dmat4", glm::dvec4);
AUTO_IMPL_VEC2(glm::vec2, "glm::vec2");
AUTO_IMPL_VEC2(glm::bvec2, "glm::bvec2");
AUTO_IMPL_VEC2(glm::dvec2, "glm::dvec2");
AUTO_IMPL_VEC2(glm::ivec2, "glm::ivec2");
AUTO_IMPL_VEC2(glm::uvec2, "glm::uvec2");
AUTO_IMPL_VEC3(glm::vec3, "glm::vec3");
AUTO_IMPL_VEC3(glm::bvec3, "glm::bvec3");
AUTO_IMPL_VEC3(glm::dvec3, "glm::dvec3");
AUTO_IMPL_VEC3(glm::ivec3, "glm::ivec3");
AUTO_IMPL_VEC3(glm::uvec3, "glm::uvec3");
AUTO_IMPL_VEC4(glm::vec4, "glm::vec4");
AUTO_IMPL_VEC4(glm::dvec4, "glm::dvec4");
AUTO_IMPL_VEC4(glm::bvec4, "glm::bvec4");
AUTO_IMPL_VEC4(glm::ivec4, "glm::ivec4");
AUTO_IMPL_VEC4(glm::uvec4, "glm::uvec4");
AUTO_IMPL_VEC4(glm::quat, "glm::quat");
AUTO_IMPL_VEC4(glm::dquat, "glm::dquat");
