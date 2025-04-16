#include <doctest/doctest.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/vector.hpp>

#include "../traits/constructible.hpp"

using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::VectorTrait;

template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
static void testMat(const char* name, glm::mat<C, R, T, Q> mat, glm::vec<R, T, Q> col)
{
    using Mat = glm::mat<C, R, T, Q>;
    using Col = glm::vec<R, T, Q>;
    const Type& type = reflect<Mat>();
    CHECK(type.name() == name);
    testConstructible<Mat>(&mat);

    // The fields of the matrix must be named a, b, c, d, in that order.
    REQUIRE(type.has<FieldsTrait>());
    const auto& fields = type.get<FieldsTrait>();

    CHECK(&*fields.begin() == fields.field("a"));
    CHECK(&*++fields.begin() == fields.field("b"));

    if constexpr (C >= 3)
    {
        CHECK(&*++(++fields.begin()) == fields.field("c"));
    }

    if constexpr (C >= 4)
    {
        CHECK(&*++(++(++fields.begin())) == fields.field("d"));
    }

    // Check if the fields have the correct type.
    glm::length_t i = 0;
    for (auto [field, data] : fields.view(&mat))
    {
        REQUIRE(field->type().template is<Col>());
        CHECK(*static_cast<Col*>(data) == glm::column(mat, i));
        i += 1;
    }

    // Check if the fields are set correctly.
    i = 0;
    for (auto [field, data] : fields.view(&mat))
    {
        mat[i] = {};
        *static_cast<Col*>(data) = col;
        CHECK(col == glm::column(mat, i));
        i += 1;
    }
}

template <glm::length_t L, typename T, glm::qualifier Q>
static void testVec(const char* name, glm::vec<L, T, Q> vec)
{
    using Vec = glm::vec<L, T, Q>;
    const Type& type = reflect<Vec>();
    CHECK(type.name() == name);
    testConstructible<Vec>(&vec);

    // Must have a Vector trait.
    REQUIRE(type.has<VectorTrait>());
    const auto& vector = type.get<VectorTrait>();
    CHECK(vector.scalarType().is<T>());
    CHECK(vector.dimensions() == L);
    for (std::size_t i = 0; i < static_cast<std::size_t>(L); ++i)
    {
        CHECK(static_cast<const T*>(vector.get(&vec, i)) == &vec[static_cast<glm::length_t>(i)]);
    }

    // The fields of the vector must be named x, y, z, w, in that order.
    REQUIRE(type.has<FieldsTrait>());
    const auto& fields = type.get<FieldsTrait>();

    CHECK(&*fields.begin() == fields.field("x"));
    CHECK(&*++fields.begin() == fields.field("y"));

    if constexpr (L >= 3)
    {
        CHECK(&*++(++fields.begin()) == fields.field("z"));
    }

    if constexpr (L >= 4)
    {
        CHECK(&*++(++(++fields.begin())) == fields.field("w"));
    }

    // Check if the fields have the correct type.
    glm::length_t i = 0;
    for (auto [field, data] : fields.view(&vec))
    {
        REQUIRE(field->type().template is<T>());
        CHECK(*static_cast<T*>(data) == vec[i]);
        i += 1;
    }

    // Check if the fields are set correctly.
    i = 0;
    for (auto [field, data] : fields.view(&vec))
    {
        vec = {};
        *static_cast<T*>(data) = static_cast<T>(1);
        CHECK(vec[i] == static_cast<T>(1));
        i += 1;
    }
}

template <typename T, glm::qualifier Q>
static void testQuat(const char* name, glm::tquat<T, Q> quat)
{
    using Quat = glm::tquat<T, Q>;
    const Type& type = reflect<Quat>();
    CHECK(type.name() == name);
    testConstructible<Quat>(&quat);

    // The fields of the quaternion must be named x, y, z, w in that order.
    REQUIRE(type.has<FieldsTrait>());
    const auto& fields = type.get<FieldsTrait>();

    CHECK(&*fields.begin() == fields.field("x"));
    CHECK(&*++fields.begin() == fields.field("y"));
    CHECK(&*++(++fields.begin()) == fields.field("z"));
    CHECK(&*++(++(++fields.begin())) == fields.field("w"));

    auto view = fields.view(&quat);

    // Check if the fields have the correct type.
    for (const auto& field : fields)
    {
        REQUIRE(field.type().template is<T>());
    }

    CHECK(*static_cast<T*>(view.get("x")) == quat.x);
    CHECK(*static_cast<T*>(view.get("y")) == quat.y);
    CHECK(*static_cast<T*>(view.get("z")) == quat.z);
    CHECK(*static_cast<T*>(view.get("w")) == quat.w);

    quat = {};
    *static_cast<T*>(view.get("x")) = static_cast<T>(1);
    CHECK(quat.x == static_cast<T>(1));

    quat = {};
    *static_cast<T*>(view.get("y")) = static_cast<T>(1);
    CHECK(quat.y == static_cast<T>(1));

    quat = {};
    *static_cast<T*>(view.get("z")) = static_cast<T>(1);
    CHECK(quat.z == static_cast<T>(1));

    quat = {};
    *static_cast<T*>(view.get("w")) = static_cast<T>(1);
    CHECK(quat.w == static_cast<T>(1));
}

TEST_CASE("reflection::reflect<(glm)>()")
{
    testMat("glm::mat2", glm::mat2{0.0F}, glm::vec2{1.0F, 2.0F});
    testMat("glm::dmat2", glm::dmat2{0.0}, glm::dvec2{1.0, 2.0});
    testMat("glm::mat2x3", glm::mat2x3{0.0F}, glm::vec3{1.0F, 2.0F, 3.0F});
    testMat("glm::dmat2x3", glm::dmat2x3{0.0}, glm::dvec3{1.0, 2.0, 3.0});
    testMat("glm::mat2x4", glm::mat2x4{0.0F}, glm::vec4{1.0F, 2.0F, 3.0F, 4.0F});
    testMat("glm::dmat2x4", glm::dmat2x4{0.0}, glm::dvec4{1.0, 2.0, 3.0, 4.0});
    testMat("glm::mat3x2", glm::mat3x2{0.0F}, glm::vec2{1.0F, 2.0F});
    testMat("glm::dmat3x2", glm::dmat3x2{0.0}, glm::dvec2{1.0, 2.0});
    testMat("glm::mat3", glm::mat3{0.0F}, glm::vec3{1.0F, 2.0F, 3.0F});
    testMat("glm::dmat3", glm::dmat3{0.0}, glm::dvec3{1.0, 2.0, 3.0});
    testMat("glm::mat3x4", glm::mat3x4{0.0F}, glm::vec4{1.0F, 2.0F, 3.0F, 4.0F});
    testMat("glm::dmat3x4", glm::dmat3x4{0.0}, glm::dvec4{1.0, 2.0, 3.0, 4.0});
    testMat("glm::mat4", glm::mat4{0.0F}, glm::vec4{1.0F, 2.0F, 3.0F, 4.0F});
    testMat("glm::dmat4", glm::dmat4{0.0}, glm::dvec4{1.0, 2.0, 3.0, 4.0});

    testVec("glm::vec2", glm::vec2{0.1, 0.2});
    testVec("glm::bvec2", glm::bvec2{true, false});
    testVec("glm::dvec2", glm::dvec2{1.2, 3.4});
    testVec("glm::ivec2", glm::ivec2{2, 1});
    testVec("glm::vec3", glm::vec3{0.1, 0.2, 0.3});
    testVec("glm::bvec3", glm::bvec3{false, true, false});
    testVec("glm::dvec3", glm::dvec3{1.2, 3.4, 5.6});
    testVec("glm::ivec3", glm::ivec3{3, 2, 1});
    testVec("glm::vec4", glm::vec4{0.1, 0.2, 0.3, 0.4});
    testVec("glm::bvec4", glm::bvec4{true, false, true, false});
    testVec("glm::dvec4", glm::dvec4{1.2, 3.4, 5.6, 7.8});
    testVec("glm::ivec4", glm::ivec4{4, 3, 2, 1});

    testQuat("glm::quat", glm::quat(0.1F, 0.05F, 0.025F, 0.0125F));
    testQuat("glm::dquat", glm::dquat(1.0, 0.5, 0.25, 0.125));
}
