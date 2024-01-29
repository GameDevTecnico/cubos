/// [Include Components]
#include "components.hpp"
/// [Include Components]

/// [Include]
#include <cubos/engine/prelude.hpp>

using cubos::engine::Cubos;
/// [Include]

using namespace cubos::engine;

/// [Component Refl]
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(Num)
{
    return cubos::core::ecs::TypeBuilder<Num>("Num").withField("value", &Num::value).build();
}
/// [Component Refl]

/// [Resource Decl]
struct Pop
{
    int count;
};
/// [Resource Decl]

/// [Hello Cubos]
static void sayHelloCubosSystem()
{
    CUBOS_INFO("Hello CUBOS");
}
/// [Hello Cubos]

/// [Hello World]
static void sayHelloSystem()
{
    CUBOS_INFO("Hello");
}

static void sayWorldSystem()
{
    CUBOS_INFO("World");
}
/// [Hello World]

/// [Entity Spawn]
static void spawnEntitiesSystem(Commands cmds, Pop& pop)
{
    for (int i = 0; i < 10; i++)
    {
        cmds.create().add(Num{i});
        pop.count += 1;
    }
}
/// [Entity Spawn]

/// [Entity Print]
static void checkEntitiesSystem(Query<const Num&> query, const Pop& pop)
{
    for (auto [num] : query)
    {
        CUBOS_INFO("Entity '{}' of '{}'", num.value, pop.count);
    }
}
/// [Entity Print]

/// [Engine]
int main()
{
    Cubos cubos{};
    /// [Engine]

    /// [Tags]
    cubos.tag("helloTag").before("worldTag");
    /// [Tags]

    /// [Set Startup]
    cubos.startupSystem(sayHelloCubosSystem);
    /// [Set Startup]

    /// [Set Systems]
    cubos.system(sayHelloSystem).tagged("helloTag");
    cubos.system(sayWorldSystem).tagged("worldTag");
    /// [Set Systems]

    /// [Component Add]
    cubos.addComponent<Num>();
    /// [Component Add]

    /// [Resource Add]
    cubos.addResource<Pop>();
    /// [Resource Add]

    /// [Entity System]
    cubos.startupSystem(spawnEntitiesSystem);

    cubos.system(checkEntitiesSystem);
    /// [Entity System]

    /// [Run]
    cubos.run();
}
/// [Run]
