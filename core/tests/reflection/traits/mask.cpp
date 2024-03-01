#include <doctest/doctest.h>

#include <cubos/core/reflection/traits/mask.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::MaskTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

namespace
{
    enum class Permissions
    {
        None = 0,
        Read = 1,
        Write = 2,
        Execute = 4
    };

    inline Permissions operator~(Permissions p)
    {
        return static_cast<Permissions>(~static_cast<int>(p));
    }

    inline Permissions operator|(Permissions a, Permissions b)
    {
        return static_cast<Permissions>(static_cast<int>(a) | static_cast<int>(b));
    }

    inline Permissions operator&(Permissions a, Permissions b)
    {
        return static_cast<Permissions>(static_cast<int>(a) & static_cast<int>(b));
    }
} // namespace

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_EMPTY, Permissions);
CUBOS_REFLECT_EXTERNAL_IMPL(Permissions)
{
    return Type::create("Permissions")
        .with(MaskTrait{}
                  .withBit<Permissions::Read>("Read")
                  .withBit<Permissions::Write>("Write")
                  .withBit<Permissions::Execute>("Execute"));
}

TEST_CASE("reflection::MaskTrait")
{
    Permissions p;

    const auto& permissionsType = reflect<Permissions>();
    REQUIRE(permissionsType.has<MaskTrait>());

    const auto& mask = permissionsType.get<MaskTrait>();
    REQUIRE(mask.contains("Read"));
    REQUIRE(mask.contains("Write"));
    REQUIRE(mask.contains("Execute"));
    REQUIRE_FALSE(mask.contains("None"));

    auto it = mask.begin();
    CHECK(it->name() == "Read");
    CHECK(it != mask.end());
    CHECK(++it != mask.end());
    CHECK(++it != mask.end());
    CHECK(++it == mask.end());

    p = Permissions::Read | Permissions::Write;
    REQUIRE(mask.view(&p).test("Read"));
    REQUIRE(mask.view(&p).test("Write"));
    REQUIRE_FALSE(mask.view(&p).test("Execute"));

    mask.view(&p).clear("Execute");
    REQUIRE(mask.view(&p).test("Read"));
    REQUIRE(mask.view(&p).test("Write"));
    REQUIRE_FALSE(mask.view(&p).test("Execute"));

    mask.view(&p).set("Execute");
    REQUIRE(mask.view(&p).test("Read"));
    REQUIRE(mask.view(&p).test("Write"));
    REQUIRE(mask.view(&p).test("Execute"));

    mask.view(&p).clear("Read");
    REQUIRE_FALSE(mask.view(&p).test("Read"));
    REQUIRE(mask.view(&p).test("Write"));
    REQUIRE(mask.view(&p).test("Execute"));

    mask.view(&p).set("Write");
    REQUIRE_FALSE(mask.view(&p).test("Read"));
    REQUIRE(mask.view(&p).test("Write"));
    REQUIRE(mask.view(&p).test("Execute"));
}
