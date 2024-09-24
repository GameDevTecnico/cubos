#include "player.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(Player)
{
    return cubos::core::ecs::TypeBuilder<Player>("Player")
        .withField("speed", &Player::speed)
        .withField("laneWidth", &Player::laneWidth)
        .build();
}

void playerPlugin(Cubos& cubos)
{
    cubos.depends(inputPlugin);
    cubos.depends(transformPlugin);

    cubos.component<Player>();

    cubos.system("move player").call([](Input& input, const DeltaTime& dt, Query<Player&, Position&> players) {
        for (auto [player, position] : players)
        {
            if (input.pressed("left") && player.lane == player.targetLane)
            {
                player.targetLane = glm::clamp(player.lane - 1, -1, 1);
            }

            if (input.pressed("right") && player.lane == player.targetLane)
            {
                player.targetLane = glm::clamp(player.lane + 1, -1, 1);
            }

            if (player.lane != player.targetLane)
            {
                auto sourceX = static_cast<float>(-player.lane) * player.laneWidth;
                auto targetX = static_cast<float>(-player.targetLane) * player.laneWidth;
                float currentT = (position.vec.x - sourceX) / (targetX - sourceX);
                float newT = glm::min(1.0F, currentT + dt.value() * player.speed);
                position.vec.x = glm::mix(sourceX, targetX, newT);
                position.vec.y = glm::sin(currentT * glm::pi<float>()) * 2.0F;

                if (newT == 1.0F)
                {
                    player.lane = player.targetLane;
                }
            }
            else
            {
                position.vec.y = 0;
            }
        }
    });
}
