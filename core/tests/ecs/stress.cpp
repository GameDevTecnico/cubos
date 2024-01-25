#include <doctest/doctest.h>

#include "utils/action.hpp"

using namespace test::ecs;

static Action* randomAction(ExpectedWorld& expected)
{
    Action* (*generators[])(ExpectedWorld&) = {
        CreateAction::random,
        DestroyAction::random,
        AddAction::random,
        RemoveAction::random,
        RelateAction::random,
        UnrelateAction::random,
        SingleTargetQueryAction::random,
        SingleRelationQueryAction::random,
    };

    Action* result = nullptr;

    while (result == nullptr)
    {
        std::size_t index = static_cast<std::size_t>(rand()) % (sizeof(generators) / sizeof(generators[0]));
        result = generators[index](expected);
    }

    return result;
}

// NOLINTBEGIN(readability-function-size)
TEST_CASE("ecs::* stress test")
{
    ExpectedWorld expected{};
    World world{};

    // Register the types.
    world.registerComponent(expected.registerComponent("cA"));
    world.registerComponent(expected.registerComponent("cB"));
    world.registerComponent(expected.registerComponent("cC"));
    world.registerComponent(expected.registerComponent("cD"));
    world.registerComponent(expected.registerComponent("cE"));
    world.registerComponent(expected.registerComponent("cF"));
    world.registerRelation(expected.registerRelation("rA"));
    world.registerRelation(expected.registerSymmetricRelation("rB"));
    world.registerRelation(expected.registerTreeRelation("rC"));
    expected.testTypes(world);

    // Execute random actions.
    std::string descriptions;
    for (int i = 0; i < 1000; ++i)
    {
        auto* action = randomAction(expected);
        descriptions += action->description() + '\n';
        INFO(descriptions);
        action->test(world, expected);
        delete action;
    }

    INFO(descriptions);
    expected.testEntities(world);
}
// NOLINTEND(readability-function-size)
