#include <doctest/doctest.h>

#include <cubos/core/tel/metrics.hpp>
#include <cubos/core/tel/tracing.hpp>

using cubos::core::tel::Metrics;
using cubos::core::tel::SpanManager;

TEST_CASE("tel::tracing")
{
    CUBOS_SPAN_DEBUG("test");
    CHECK(SpanManager::current().path.find("test") > 0);
    CHECK(SpanManager::current().id == 1);

    {
        CUBOS_SPAN_DEBUG("test_again");
        CHECK(SpanManager::current().path.find("test_again") > 0);
        CHECK(SpanManager::current().id == 2);
    }

    CHECK(SpanManager::current().path.find("test") > 0);
    CHECK(SpanManager::current().id == 1);

#ifdef CUBOS_PROFILING
    std::string span;
    std::size_t seen = 0;
    Metrics::readName(span, seen);
    CHECK(span.find("test") > 0);
    Metrics::clear();
#endif
}