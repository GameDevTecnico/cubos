#include <doctest/doctest.h>

#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

/// @brief Type which is reflected internally.
struct Internal
{
    CUBOS_REFLECT;
};

CUBOS_REFLECT_IMPL(Internal)
{
    return Type::create("Internal");
}

/// @brief Type which is reflected externally.
struct External
{
};

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_EMPTY, External);
CUBOS_REFLECT_EXTERNAL_IMPL(External)
{
    return Type::create("External");
}

/// @brief Templated type.
template <typename T>
struct Templated
{
};

CUBOS_REFLECT_EXTERNAL_TEMPLATE((typename T), (Templated<T>))
{
    return Type::create("Templated<" + reflect<T>().name() + ">");
}

/// @brief Type with anonymous reflection.
struct Anonymous
{
    CUBOS_ANONYMOUS_REFLECT(Anonymous);
};

TEST_CASE("reflection::reflect")
{
    CHECK(reflect<Internal>().name() == "Internal");
    CHECK(reflect<Internal>().is<Internal>());

    CHECK(reflect<External>().name() == "External");
    CHECK(reflect<External>().is<External>());
    CHECK_FALSE(reflect<External>().is<Internal>());

    CHECK(reflect<Templated<Internal>>().name() == "Templated<Internal>");
    CHECK(reflect<Templated<External>>().name() == "Templated<External>");
    CHECK(reflect<Templated<Templated<Internal>>>().name() == "Templated<Templated<Internal>>");

    CHECK(reflect<Anonymous>().is<Anonymous>());
    CHECK(reflect<Anonymous>().has<ConstructibleTrait>());
    CHECK(reflect<Anonymous>().get<ConstructibleTrait>().size() == sizeof(Anonymous));
    CHECK(reflect<Anonymous>().get<ConstructibleTrait>().alignment() == alignof(Anonymous));
}
