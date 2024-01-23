#include <cubos/core/ecs/query/data.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/ecs/world.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>

using cubos::core::ecs::Entity;
using cubos::core::ecs::QueryData;
using cubos::core::ecs::QueryTerm;
using cubos::core::ecs::TypeBuilder;
using cubos::core::ecs::World;
using cubos::core::reflection::reflect;

struct ChildOf
{
    CUBOS_REFLECT;
};

CUBOS_REFLECT_IMPL(ChildOf)
{
    return TypeBuilder<ChildOf>("ChildOf").build();
}

struct Name
{
    CUBOS_REFLECT;
    std::string name;
};

CUBOS_REFLECT_IMPL(Name)
{
    return TypeBuilder<Name>("Name").withField("name", &Name::name).build();
}

int main()
{
    World world{};
    world.registerRelation<ChildOf>();
    world.registerComponent<Name>();
    auto childOfRelation = world.types().id(reflect<ChildOf>());
    auto nameComponent = world.types().id(reflect<Name>());

    auto alice = world.create();
    world.components(alice).add(Name{"Alice"});
    auto bob = world.create();
    world.components(bob).add(Name{"Bob"});
    auto charlie = world.create();
    world.components(charlie).add(Name{"Charlie"});
    auto diego = world.create();
    world.components(diego).add(Name{"Diego"});
    auto eve = world.create();
    world.components(eve).add(Name{"Eve"});

    world.relate(alice, charlie, ChildOf{});
    world.relate(bob, charlie, ChildOf{});
    world.relate(charlie, eve, ChildOf{});
    world.relate(diego, eve, ChildOf{});

    QueryData<const Name&, const Name&> query{world,
                                    {
                                        QueryTerm::makeWithComponent(nameComponent, 0),
                                        QueryTerm::makeRelation(childOfRelation, 0, 1),
                                        QueryTerm::makeWithComponent(nameComponent, 1),
                                    }};

    CUBOS_INFO("Query over all relations:");
    for (auto [child, parent] : query.view())
    {
        CUBOS_INFO("{} is a child of {}", child, parent);
    }
}
