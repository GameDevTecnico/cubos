#include <doctest/doctest.h>

#include <cubos/core/reflection/traits/nullable.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::NullableTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

namespace
{
    struct MyEntity
    {
        CUBOS_REFLECT;
        uint32_t idx;
        uint32_t generation;
    };
} // namespace

CUBOS_REFLECT_IMPL(MyEntity)
{
    return Type::create("MyEntity")
        .with(NullableTrait{[](const void* instance) {
                                const auto* ent = static_cast<const MyEntity*>(instance);
                                return ent->idx == UINT32_MAX && ent->generation == UINT32_MAX;
                            },
                            [](void* instance) {
                                auto* ent = static_cast<MyEntity*>(instance);
                                ent->idx = UINT32_MAX;
                                ent->generation = UINT32_MAX;
                            }});
}

TEST_CASE("reflection::NullableTrait")
{
    const auto& entityType = reflect<MyEntity>();
    REQUIRE(entityType.has<NullableTrait>());

    const auto& nullableTrait = entityType.get<NullableTrait>();

    MyEntity ent{1, 1};
    REQUIRE(!nullableTrait.isNull(&ent));

    nullableTrait.setToNull(&ent);
    REQUIRE(nullableTrait.isNull(&ent));
}
