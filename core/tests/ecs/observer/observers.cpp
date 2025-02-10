#include <doctest/doctest.h>

#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/observer/observers.hpp>

#include "../utils.hpp"

using namespace cubos::core::ecs;

TEST_CASE("ecs::Observers")
{
    World world{};
    CommandBuffer cmdBuffer{world};
    Observers obs{};

    static int acc = 0;
    auto hook1 = obs.hookOnAdd(ColumnId{.inner = 1}, System<void>::make(world, []() { acc += 1; }, {}));
    auto hook2 = obs.hookOnRemove(ColumnId{.inner = 1}, System<void>::make(world, []() { acc -= 1; }, {}));
    auto hook3 = obs.hookOnDestroy(ColumnId{.inner = 1}, System<void>::make(world, []() { acc -= 2; }, {}));
    CHECK(acc == 0);

    obs.notifyAdd(cmdBuffer, Entity{0, 0}, ColumnId{.inner = 0});
    CHECK(acc == 0);
    obs.notifyRemove(cmdBuffer, Entity{0, 0}, ColumnId{.inner = 2});
    CHECK(acc == 0);
    obs.notifyDestroy(cmdBuffer, Entity{0, 0}, ColumnId{.inner = 2});
    CHECK(acc == 0);

    obs.notifyAdd(cmdBuffer, Entity{0, 0}, ColumnId{.inner = 1});
    CHECK(acc == 1);
    obs.notifyRemove(cmdBuffer, Entity{0, 0}, ColumnId{.inner = 1});
    CHECK(acc == 0);
    obs.notifyDestroy(cmdBuffer, Entity{0, 0}, ColumnId{.inner = 1});
    CHECK(acc == -2);

    obs.unhook(hook2);
    obs.notifyAdd(cmdBuffer, Entity{0, 0}, ColumnId{.inner = 1});
    CHECK(acc == -1);
    obs.notifyRemove(cmdBuffer, Entity{0, 0}, ColumnId{.inner = 1});
    CHECK(acc == -1);

    obs.unhook(hook3);
    obs.notifyAdd(cmdBuffer, Entity{0, 0}, ColumnId{.inner = 1});
    CHECK(acc == 0);
    obs.notifyDestroy(cmdBuffer, Entity{0, 0}, ColumnId{.inner = 1});
    CHECK(acc == 0);

    obs.unhook(hook1);
    obs.notifyAdd(cmdBuffer, Entity{0, 0}, ColumnId{.inner = 1});
    CHECK(acc == 0);
}
