#include <cubos/core/ecs/world.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::ecs::World;
using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::Type;

struct ChildOf
{
    CUBOS_REFLECT;
};

CUBOS_REFLECT_IMPL(ChildOf)
{
    return Type::create("ChildOf").with(ConstructibleTrait::typed<ChildOf>().withMoveConstructor().build());
}

int main()
{
    World world{};
    world.registerRelation<ChildOf>();

    auto foo = world.create();
    auto bar = world.create();

    CUBOS_ASSERT(!world.related<ChildOf>(foo, bar), "foo and bar should not be related yet");
    world.relate(foo, bar, ChildOf{});
    CUBOS_ASSERT(world.related<ChildOf>(foo, bar), "foo and bar should now be related");
    world.unrelate<ChildOf>(foo, bar);
    CUBOS_ASSERT(!world.related<ChildOf>(foo, bar), "foo and bar should no longer be related");
}
