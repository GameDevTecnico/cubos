#include <doctest/doctest.h>

#include <cubos/core/reflection/traits/wrapper.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;
using cubos::core::reflection::WrapperTrait;

namespace
{
    struct InnerType
    {
        CUBOS_REFLECT;
    };

    CUBOS_REFLECT_IMPL(InnerType)
    {
        return Type::create("InnerType");
    }

    struct MyWrapperType
    {
        CUBOS_REFLECT;
        InnerType inner;
    };

    CUBOS_REFLECT_IMPL(MyWrapperType)
    {
        return Type::create("MyWrapperType").with(WrapperTrait{&MyWrapperType::inner});
    }
} // namespace

TEST_CASE("reflection::WrapperTrait")
{
    MyWrapperType val{InnerType{}};
    const auto& type = reflect<MyWrapperType>();
    REQUIRE(type.has<WrapperTrait>());
    const auto& trait = type.get<WrapperTrait>();
    CHECK(trait.type().is<InnerType>());
    CHECK(trait.value(&val) == &val.inner);
}
