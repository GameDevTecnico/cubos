#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/object.hpp>

#include "../utils.hpp"

using cubos::core::reflection::ObjectType;
using cubos::core::reflection::reflect;

template <glm::length_t L, typename T, glm::qualifier Q>
static void testVec(glm::vec<L, T, Q>, const char* name)
{
    using Vec = glm::vec<L, T, Q>;
    testTypeGetters<Vec, ObjectType>(name, name);
    testTypeDefaultConstructor<Vec>();

    // The fields of vector must be named x, y, z, w, in that order.
    auto& to = reflect<Vec>().template asKind<ObjectType>();
    REQUIRE(to.fields().size() == static_cast<std::size_t>(L));

    CHECK(to.fields()[0]->name() == "x");
    CHECK(to.fields()[1]->name() == "y");

    if (L >= 3)
    {
        CHECK(to.fields()[2]->name() == "z");
    }

    if (L >= 4)
    {
        CHECK(to.fields()[3]->name() == "w");
    }

    // Check if the correct field types are returned.
    for (auto& field : to.fields())
    {
        REQUIRE(field->type().template is<T>());
    }

    // Check if the fields are set correctly.
    for (glm::length_t i = 0; i < L; ++i)
    {
        Vec vec{};
        to.fields()[i]->template get<T>(&vec) = static_cast<T>(1);
        CHECK(vec[i] == static_cast<T>(1));
    }
}

template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
static void testMat(glm::mat<C, R, T, Q>, const char* name)
{
    using Mat = glm::mat<C, R, T, Q>;
    testTypeGetters<Mat, ObjectType>(name, name);
    testTypeDefaultConstructor<Mat>();

    // The fields of matrix must be named 00, 01, 02, 10, 11, etc, in that order.
    auto& to = reflect<Mat>().template asKind<ObjectType>();
    REQUIRE(to.fields().size() == static_cast<std::size_t>(C * R));
    for (glm::length_t c = 0; c < C; ++c)
    {
        for (glm::length_t r = 0; r < R; ++r)
        {
            std::string expected = std::to_string(c) + std::to_string(r);
            CHECK(to.fields()[c * R + r]->name() == expected);
            REQUIRE(to.fields()[c * R + r]->type().template is<T>());
        }
    }

    // Check if the fields are set correctly.
    for (glm::length_t c = 0; c < C; ++c)
    {
        for (glm::length_t r = 0; r < R; ++r)
        {
            Mat mat{};
            to.fields()[c * R + r]->template get<T>(&mat) = static_cast<T>(1);
            CHECK(mat[c][r] == static_cast<T>(1));
        }
    }
}

template <typename T, glm::qualifier Q>
static void testQuat(glm::qua<T, Q>, const char* name)
{
    using Quat = glm::qua<T, Q>;
    testTypeGetters<Quat, ObjectType>(name, name);
    testTypeDefaultConstructor<Quat>();

    // The fields of quaternion must be named x, y, z, w, in that order.
    auto& to = reflect<Quat>().template asKind<ObjectType>();
    REQUIRE(to.fields().size() == 4);
    CHECK(to.fields()[0]->name() == "x");
    CHECK(to.fields()[1]->name() == "y");
    CHECK(to.fields()[2]->name() == "z");
    CHECK(to.fields()[3]->name() == "w");

    // Check if the correct field types are returned.
    for (auto& field : to.fields())
    {
        REQUIRE(field->type().template is<T>());
    }

    // Check if the fields are set correctly.
    for (glm::length_t i = 0; i < 4; ++i)
    {
        Quat qua{};
        to.fields()[i]->template get<T>(&qua) = static_cast<T>(1);
        CHECK(qua[i] == static_cast<T>(1));
    }
}

TEST_CASE("reflection::reflect<(external/glm)>()")
{
    testVec(glm::ivec2{}, "glm::ivec2");
    testVec(glm::uvec2{}, "glm::uvec2");
    testVec(glm::vec2{}, "glm::vec2");

    testVec(glm::ivec3{}, "glm::ivec3");
    testVec(glm::uvec3{}, "glm::uvec3");
    testVec(glm::vec3{}, "glm::vec3");

    testVec(glm::ivec4{}, "glm::ivec4");
    testVec(glm::uvec4{}, "glm::uvec4");
    testVec(glm::vec4{}, "glm::vec4");

    testMat(glm::mat2{}, "glm::mat2");

    testMat(glm::mat3{}, "glm::mat3");

    testMat(glm::mat4{}, "glm::mat4");

    testQuat(glm::quat{}, "glm::quat");
}
