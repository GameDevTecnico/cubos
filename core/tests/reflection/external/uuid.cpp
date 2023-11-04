#include <doctest/doctest.h>

#include <cubos/core/reflection/external/uuid.hpp>

#include "../traits/constructible.hpp"
#include "../traits/string_conversion.hpp"

TEST_CASE("reflection::reflect<uuids::uuid>()")
{
    uuids::uuid id = uuids::uuid::from_string("61a10fbd-899e-417d-9ef5-c83d8f6643ba").value();
    CHECK(reflect<uuids::uuid>().name() == "uuids::uuid");
    testConstructible<uuids::uuid>(&id);
    testStringConversion<uuids::uuid>(
        {
            {*uuids::uuid::from_string("00000000-0000-0000-0000-000000000000"), "00000000-0000-0000-0000-000000000000"},
            {*uuids::uuid::from_string("61a10fbd-899e-417d-9ef5-c83d8f6643ba"), "61a10fbd-899e-417d-9ef5-c83d8f6643ba"},
        },
        {
            "foo",
            "61a10fbd-899e-417d-9ef5-c83d8f6643b",
            "61a10fbd-899e-417d-9ef5-c83d8f6643bab",
        });
}
