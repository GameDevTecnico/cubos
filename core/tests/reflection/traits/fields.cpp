#include <doctest/doctest.h>

#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

struct SimpleType
{
    CUBOS_REFLECT
    {
        return Type::create("SimpleType");
    }
};

struct ObjectType
{
    SimpleType foo;
    SimpleType bar;
};

TEST_CASE("reflection::FieldsTrait")
{
    SUBCASE("no fields")
    {
        auto fields = FieldsTrait();
        CHECK(fields.field("foo") == nullptr);
        CHECK(fields.firstField() == nullptr);
    }

    SUBCASE("single field")
    {
        auto fields = FieldsTrait().withField("foo", &ObjectType::foo);

        auto field = fields.field("foo");
        REQUIRE(field != nullptr);
        CHECK(field == fields.firstField());
        CHECK(field->next() == nullptr);

        ObjectType object{};
        CHECK(field->name() == "foo");
        CHECK(field->type().is<SimpleType>());
        CHECK(field->pointerTo(&object) == &object.foo);
    }

    SUBCASE("two fields")
    {
        auto fields = FieldsTrait().withField("foo", &ObjectType::foo).withField("bar", &ObjectType::bar);

        auto fooField = fields.field("foo");
        REQUIRE(fooField != nullptr);
        CHECK(fooField == fields.firstField());

        auto barField = fields.field("bar");
        REQUIRE(barField != nullptr);
        CHECK(fooField->next() == barField);
        CHECK(barField->next() == nullptr);

        ObjectType object{};
        CHECK(fooField->name() == "foo");
        CHECK(fooField->type().is<SimpleType>());
        CHECK(fooField->pointerTo(&object) == &object.foo);
        CHECK(barField->name() == "bar");
        CHECK(barField->type().is<SimpleType>());
        CHECK(barField->pointerTo(static_cast<const ObjectType*>(&object)) == &object.bar);
    }
}
