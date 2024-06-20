#include <doctest/doctest.h>
#include <glm/glm.hpp>

#include <cubos/core/reflection/comparison.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/reflection/type_registry.hpp>
#include <cubos/core/tel/logging.hpp>

using cubos::core::reflection::compare;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

struct FieldsObject
{
    CUBOS_REFLECT;
    int age;
    float weight;
};

CUBOS_REFLECT_IMPL(FieldsObject)
{
    return Type::create("FieldsObject")
        .with(FieldsTrait().withField("age", &FieldsObject::age).withField("weight", &FieldsObject::weight));
}

TEST_CASE("reflection::compare")
{
    const auto fo = FieldsObject{1, 0.3F};
    const auto foEqual = FieldsObject{1, 0.3F};
    const auto foDifferent = FieldsObject{2, 0.3F};
    CHECK(compare(reflect<FieldsObject>(), (void*)&fo, (void*)&foEqual));
    CHECK_FALSE(compare(reflect<FieldsObject>(), (void*)&fo, (void*)&foDifferent));

    std::vector<std::unordered_map<int, glm::vec3>> vec = {
        std::unordered_map<int, glm::vec3>{{1, {1.2, 2.3, 4.5}}, {2, {1.2, 1.3, 4.5}}, {560, {1.2, 2.3, 4.5}}},
        std::unordered_map<int, glm::vec3>{{5, {1.2, 2.3, 4.5}}},
        std::unordered_map<int, glm::vec3>{{6, {1.2, 2.5, 4.5}}}};

    std::vector<std::unordered_map<int, glm::vec3>> vecEqual = {
        std::unordered_map<int, glm::vec3>{{1, {1.2, 2.3, 4.5}}, {2, {1.2, 1.3, 4.5}}, {560, {1.2, 2.3, 4.5}}},
        std::unordered_map<int, glm::vec3>{{5, {1.2, 2.3, 4.5}}},
        std::unordered_map<int, glm::vec3>{{6, {1.2, 2.5, 4.5}}}};

    std::vector<std::unordered_map<int, glm::vec3>> vecDiff = {
        std::unordered_map<int, glm::vec3>{{1, {1.2, 2.31, 4.5}}, {2, {1.2, 1.3, 4.5}}, {560, {1.2, 2.3, 4.5}}},
        std::unordered_map<int, glm::vec3>{{5, {1.2, 2.3, 4.5}}},
        std::unordered_map<int, glm::vec3>{{6, {1.2, 2.5, 4.5}}}};

    CHECK(compare(reflect<std::vector<std::unordered_map<int, glm::vec3>>>(), (void*)&vec, (void*)&vecEqual));
    CHECK_FALSE(compare(reflect<std::vector<std::unordered_map<int, glm::vec3>>>(), (void*)&vec, (void*)&vecDiff));
}
