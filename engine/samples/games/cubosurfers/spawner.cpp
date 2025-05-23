#include "spawner.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(Spawner)
{
    return cubos::core::ecs::TypeBuilder<Spawner>("Spawner")
        .withField("scene", &Spawner::scene)
        .withField("period", &Spawner::period)
        .withField("laneWidth", &Spawner::laneWidth)
        .withField("accumulator", &Spawner::accumulator)
        .build();
}

void spawnerPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);

    cubos.component<Spawner>();

    cubos.system("spawn stuff")
        .call([](Commands commands, const DeltaTime& dt, Assets& assets, Query<Spawner&, const Position&> spawners) {
            for (auto [spawner, position] : spawners)
            {
                spawner.accumulator += dt.value();
                if (spawner.accumulator >= spawner.period)
                {
                    spawner.accumulator -= spawner.period;

                    Position spawnPosition = position;
                    int offset = (rand() % 3) - 1;
                    spawnPosition.vec.x += static_cast<float>(offset) * spawner.laneWidth;

                    commands.spawn(assets.read(spawner.scene)->blueprint()).add(spawnPosition);
                }
            }
        });
}
