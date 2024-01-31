/// [Include]
#include <cubos/engine/prelude.hpp>

using namespace cubos::engine;
/// [Include]

/// [Component Decl]
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

struct Num
{
    CUBOS_REFLECT;

    int value;
};

CUBOS_REFLECT_IMPL(Num)
{
    return cubos::core::ecs::TypeBuilder<Num>("Num").withField("value", &Num::value).build();
}
/// [Component Decl]

/// [Resource Decl]
struct Pop
{
    int count;
};
/// [Resource Decl]

/// [Engine]
int main()
{
    Cubos cubos{};
    /// [Engine]

    /// [Hello CUBOS]
    cubos.startupSystem("say Hello CUBOS").call([]() { CUBOS_INFO("Hello CUBOS"); });
    /// [Hello CUBOS]

    /// [Hello World]
    cubos.system("say Hello").tagged("helloTag").call([]() { CUBOS_INFO("Hello"); });
    cubos.system("say World").tagged("worldTag").call([]() { CUBOS_INFO("World"); });
    /// [Hello World]

    /// [Tags]
    cubos.tag("helloTag").before("worldTag");
    /// [Tags]

    /// [Component Add]
    cubos.addComponent<Num>();
    /// [Component Add]

    /// [Resource Add]
    cubos.addResource<Pop>();
    /// [Resource Add]

    /// [Entity Spawn]
    cubos.startupSystem("spawn entities").call([](Commands cmds, Pop& pop) {
        for (int i = 0; i < 10; i++)
        {
            cmds.create().add(Num{i});
            pop.count += 1;
        }
    });
    /// [Entity Spawn]

    /// [Entity Print]
    cubos.system("check entities").call([](Query<const Num&> query, const Pop& pop) {
        for (auto [num] : query)
        {
            CUBOS_INFO("Entity '{}' of '{}'", num.value, pop.count);
        }
    });
    /// [Entity Print]

    /// [Run]
    cubos.run();
}
/// [Run]
