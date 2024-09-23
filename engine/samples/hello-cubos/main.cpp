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
    CUBOS_REFLECT;

    int count = 0;
};

CUBOS_REFLECT_IMPL(Pop)
{
    return cubos::core::ecs::TypeBuilder<Pop>("Pop").withField("count", &Pop::count).build();
}
/// [Resource Decl]

namespace cubos::engine
{
    extern Tag helloTag;
    extern Tag worldTag;
} // namespace cubos::engine

CUBOS_DEFINE_TAG(cubos::engine::helloTag);
CUBOS_DEFINE_TAG(cubos::engine::worldTag);

/// [Engine]
int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    /// [Engine]

    /// [Hello Cubos]
    cubos.startupSystem("say Hello Cubos").call([]() { CUBOS_INFO("Hello Cubos"); });
    /// [Hello Cubos]

    /// [Tags]
    cubos.tag(helloTag);
    cubos.tag(worldTag).after(helloTag);
    /// [Tags]

    /// [Hello World]
    cubos.system("say Hello").tagged(helloTag).call([]() { CUBOS_INFO("Hello"); });
    cubos.system("say World").tagged(worldTag).call([]() { CUBOS_INFO("World"); });
    /// [Hello World]

    /// [Component Add]
    cubos.component<Num>();
    /// [Component Add]

    /// [Resource Add]
    cubos.resource<Pop>();
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
            CUBOS_INFO("Entity {} of {}", num.value, pop.count);
        }
    });
    /// [Entity Print]

    /// [Run]
    cubos.run();
}
/// [Run]
