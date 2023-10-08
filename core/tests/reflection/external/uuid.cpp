#include <doctest/doctest.h>

#include <cubos/core/reflection/external/uuid.hpp>

#include "../traits/constructible.hpp"

TEST_CASE("reflection::reflect<uuids::uuid>()")
{
    uuids::uuid id = uuids::uuid::from_string("61a10fbd-899e-417d-9ef5-c83d8f6643ba").value();
    CHECK(reflect<uuids::uuid>().name() == "uuids::uuid");
    testConstructible<uuids::uuid>(&id);
}
