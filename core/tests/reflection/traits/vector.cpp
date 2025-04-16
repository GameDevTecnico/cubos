#include <doctest/doctest.h>

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/vector.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;
using cubos::core::reflection::VectorTrait;

namespace
{
    struct Vec3
    {
        CUBOS_REFLECT;

        int x;
        int y;
        int z;
    };
} // namespace

CUBOS_REFLECT_IMPL(Vec3)
{
    return Type::create("Vec3").with(VectorTrait{reflect<int>(), 3});
}

TEST_CASE("reflection::VectorTrait")
{
    const auto& type = reflect<Vec3>();
    REQUIRE(type.has<VectorTrait>());
    const auto& trait = type.get<VectorTrait>();
    REQUIRE(trait.scalarType().is<int>());
    REQUIRE(trait.dimensions() == 3);
    auto vec = Vec3{1, 2, 3};
    CHECK(static_cast<const int*>(trait.get(&vec, 0)) == &vec.x);
    CHECK(static_cast<const int*>(trait.get(&vec, 1)) == &vec.y);
    CHECK(static_cast<const int*>(trait.get(&vec, 2)) == &vec.z);
}
