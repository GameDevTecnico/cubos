#include <doctest/doctest.h>

#include <cubos/core/ecs/entity/archetype_graph.hpp>

using cubos::core::ecs::ArchetypeGraph;
using cubos::core::ecs::ArchetypeId;
using cubos::core::ecs::DenseColumnId;

TEST_CASE("ecs::ArchetypeGraph")
{
    ArchetypeGraph graph{};
    std::vector<ArchetypeId> allWith0{};
    std::size_t seenWith0 = 0;

    DenseColumnId col0{.inner = 0};
    DenseColumnId col1{.inner = 1};

    REQUIRE_FALSE(graph.contains(ArchetypeId::Empty, col0));
    REQUIRE_FALSE(graph.contains(ArchetypeId::Empty, col1));

    auto a0 = graph.with(ArchetypeId::Empty, col0);
    REQUIRE(graph.contains(a0, col0));
    REQUIRE_FALSE(graph.contains(a0, col1));
    REQUIRE_FALSE(graph.contains(ArchetypeId::Empty, col0));
    REQUIRE(graph.with(ArchetypeId::Empty, col0) == a0);
    REQUIRE(graph.without(a0, col0) == ArchetypeId::Empty);

    seenWith0 = graph.collect(a0, allWith0, seenWith0);
    REQUIRE(allWith0.size() == 1);
    REQUIRE(allWith0[0] == a0);

    auto a01 = graph.with(a0, col1);
    REQUIRE(graph.contains(a01, col0));
    REQUIRE(graph.contains(a01, col1));
    REQUIRE(graph.with(a0, col1) == a01);

    seenWith0 = graph.collect(a0, allWith0, seenWith0);
    REQUIRE(allWith0.size() == 2);
    REQUIRE(allWith0[0] == a0);
    REQUIRE(allWith0[1] == a01);

    auto a1 = graph.without(a01, col0);
    REQUIRE(graph.contains(a1, col1));
    REQUIRE_FALSE(graph.contains(a1, col0));
    REQUIRE(graph.with(ArchetypeId::Empty, col1) == a1);
    REQUIRE(graph.without(a1, col1) == ArchetypeId::Empty);

    graph.collect(a0, allWith0, seenWith0);
    REQUIRE(allWith0.size() == 2);
    REQUIRE(allWith0[0] == a0);
    REQUIRE(allWith0[1] == a01);

    std::vector<ArchetypeId> allWith1{};
    graph.collect(a1, allWith1, 0);
    REQUIRE(allWith1.size() == 2);
    REQUIRE(allWith1[0] == a01);
    REQUIRE(allWith1[1] == a1);

    std::vector<ArchetypeId> all{};
    graph.collect(ArchetypeId::Empty, all, 0);
    REQUIRE(all.size() == 4);
    REQUIRE(all[0] == ArchetypeId::Empty);
    REQUIRE(all[1] == a0);
    REQUIRE(all[2] == a01);
    REQUIRE(all[3] == a1);
}
