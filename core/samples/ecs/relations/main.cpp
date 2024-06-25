#include <cubos/core/ecs/query/data.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/ecs/world.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/tel/logging.hpp>

using cubos::core::ecs::Entity;
using cubos::core::ecs::QueryData;
using cubos::core::ecs::QueryTerm;
using cubos::core::ecs::TypeBuilder;
using cubos::core::ecs::World;
using cubos::core::reflection::reflect;

struct ChildOf
{
    CUBOS_REFLECT;
    bool adopted = false;
};

CUBOS_REFLECT_IMPL(ChildOf)
{
    return TypeBuilder<ChildOf>("ChildOf").withField("adopted", &ChildOf::adopted).build();
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

    world.relate(alice, charlie, ChildOf{false});
    world.relate(bob, charlie, ChildOf{true});
    world.relate(charlie, eve, ChildOf{true});
    world.relate(diego, eve, ChildOf{false});

    QueryData<const Name&, const Name&> nameQuery{world,
                                                  {
                                                      QueryTerm::makeWithComponent(nameComponent, 0),
                                                      QueryTerm::makeRelation(childOfRelation, 0, 1),
                                                      QueryTerm::makeWithComponent(nameComponent, 1),
                                                  }};

    CUBOS_INFO("Query over all relations:");
    for (auto [child, parent] : nameQuery.view())
    {
        CUBOS_INFO("{} is a child of {}", child, parent);
    }

    CUBOS_INFO("Query over all parents of Charlie:");
    for (auto [child, parent] : nameQuery.view().pin(0, charlie))
    {
        CUBOS_ASSERT(child.name == "Charlie");
        CUBOS_INFO("{} is a parent of Charlie", parent);
    }

    CUBOS_INFO("Query over all children of Charlie:");
    for (auto [child, parent] : nameQuery.view().pin(1, charlie))
    {
        CUBOS_ASSERT(parent.name == "Charlie");
        CUBOS_INFO("{} is a child of Charlie", child);
    }

    QueryData<const ChildOf&> relationQuery{world, {}};
    CUBOS_INFO("Is Charlie Eve's parent?");
    if (auto result = relationQuery.at(eve, charlie))
    {
        auto& [childOf] = *result;
        CUBOS_INFO("Yes. Is she adopted?");
        if (childOf.adopted)
        {
            CUBOS_INFO("Yes.");
        }
        else
        {
            CUBOS_INFO("No.");
        }
    }
    else
    {
        CUBOS_INFO("No.");
    }

    CUBOS_INFO("Is Charlie Eve's child?");
    if (auto result = relationQuery.at(charlie, eve))
    {
        auto& [childOf] = *result;
        CUBOS_INFO("Yes. Is he adopted?");
        if (childOf.adopted)
        {
            CUBOS_INFO("Yes.");
        }
        else
        {
            CUBOS_INFO("No.");
        }
    }
    else
    {
        CUBOS_INFO("No.");
    }
}
