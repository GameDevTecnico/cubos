#include <doctest/doctest.h>

#include <cubos/core/tel/metrics.hpp>

using cubos::core::tel::Metrics;

TEST_CASE("metrics")
{
    CUBOS_METRIC("a", 1);
    CUBOS_METRIC("b", 2);
    CUBOS_METRIC("a", 3);

    CHECK(Metrics::sizeByName("a") == 2);
    CHECK(Metrics::sizeByName("b") == 1);

    // reading names
    std::size_t seenCount{0};
    std::string name{};

    CHECK(Metrics::readName(name, seenCount));
    CHECK(seenCount == 1);
    CHECK(name == "a");

    CHECK(Metrics::readName(name, seenCount));
    CHECK(seenCount == 2);
    CHECK(name == "b");

    CHECK_FALSE(Metrics::readName(name, seenCount));

    CUBOS_METRIC("a", 4);
    CHECK_FALSE(Metrics::readName(name, seenCount));

    CUBOS_METRIC("c", 5);
    CHECK(Metrics::readName(name, seenCount));
    CHECK(seenCount == 3);
    CHECK(name == "c");

    CHECK_FALSE(Metrics::readName(name, seenCount));

    // reading values
    std::size_t offset{0};
    double value;

    CHECK(Metrics::readValue("a", value, offset));
    CHECK(value == 1.0);
    CHECK(offset == 1);

    CHECK(Metrics::readValue("a", value, offset));
    CHECK(value == 3.0);
    CHECK(offset == 2);

    CHECK_FALSE(Metrics::readValue("b", value, offset));

    offset = 0;
    CHECK(Metrics::readValue("b", value, offset));
    CHECK(value == 2.0);
    CHECK(offset == 1);

    CHECK_FALSE(Metrics::readValue("b", value, offset));

    CHECK(Metrics::sizeByName("random_data") == 0);
    CHECK_FALSE(Metrics::readValue("random_data", value, offset));

    {
        CUBOS_PROFILE();
    }
    CHECK(Metrics::size() == 4);

    Metrics::clear();
    CHECK(Metrics::size() == 0);

    Metrics::setMaxEntries(2);
    CUBOS_METRIC("a", 1);
    CUBOS_METRIC("a", 2);
    CUBOS_METRIC("a", 3);
    CHECK(Metrics::sizeByName("a") == 2);
    CHECK(Metrics::size() == 1); // only "a"
}