#include <doctest/doctest.h>

#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

struct SimpleType
{
    CUBOS_REFLECT;
};

CUBOS_REFLECT_IMPL(SimpleType)
{
    return Type::create("SimpleType");
}

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
        CHECK(fields.begin() == fields.end());
        CHECK(fields.size() == 0);
    }

    SUBCASE("single field")
    {
        auto fields = FieldsTrait().withField("foo", &ObjectType::foo);

        const auto* field = fields.field("foo");
        REQUIRE(field != nullptr);
        CHECK(field == &*fields.begin());
        CHECK(fields.begin() != fields.end());
        CHECK(++fields.begin() == fields.end());
        CHECK(fields.size() == 1);

        ObjectType object{};
        CHECK(field->name() == "foo");
        CHECK(field->type().is<SimpleType>());

        auto view = fields.view(&object);
        CHECK(view.get(*field) == &object.foo);
        CHECK(view.begin() != view.end());
        CHECK(view.begin()->field == field);
        CHECK(++view.begin() == view.end());

        auto constView = fields.view(static_cast<const ObjectType*>(&object));
        CHECK(constView.get(*field) == &object.foo);
        CHECK(constView.begin() != constView.end());
        CHECK(constView.begin()->field == field);
        CHECK(++constView.begin() == constView.end());
    }

    SUBCASE("two fields")
    {
        auto fields = FieldsTrait().withField("foo", &ObjectType::foo).withField("bar", &ObjectType::bar);

        const auto* fooField = fields.field("foo");
        REQUIRE(fooField != nullptr);
        CHECK(fooField == &*fields.begin());

        const auto* barField = fields.field("bar");
        REQUIRE(barField != nullptr);
        CHECK(barField == &*(++fields.begin()));
        CHECK(++(++fields.begin()) == fields.end());
        CHECK(fields.size() == 2);

        ObjectType object{};
        CHECK(fooField->name() == "foo");
        CHECK(fooField->type().is<SimpleType>());
        CHECK(fields.view(&object).get(*fooField) == &object.foo);
        CHECK(barField->name() == "bar");
        CHECK(barField->type().is<SimpleType>());
        CHECK(fields.view(static_cast<const ObjectType*>(&object)).get(*barField) == &object.bar);

        auto view = fields.view(&object);
        CHECK(view.get(*fooField) == &object.foo);
        CHECK(view.begin() != view.end());
        CHECK(view.begin()->field == fooField);
        CHECK((++view.begin())->field == barField);
        CHECK(++(++view.begin()) == view.end());

        auto constView = fields.view(static_cast<const ObjectType*>(&object));
        CHECK(constView.get(*fooField) == &object.foo);
        CHECK(constView.begin() != constView.end());
        CHECK(constView.begin()->field == fooField);
        CHECK((++constView.begin())->field == barField);
        CHECK(++(++constView.begin()) == constView.end());
    }
}
