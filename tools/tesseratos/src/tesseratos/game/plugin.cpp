#include "plugin.hpp"

using namespace cubos::engine;

CUBOS_DEFINE_TAG(tesseratos::gameUpdateTag);

void tesseratos::gamePlugin(Cubos& cubos)
{
    cubos.resource<Game>();

    cubos.tag(gameUpdateTag);

    cubos.system("update Game").tagged(gameUpdateTag).call([](Game& game) {
        if (game.isStarted())
        {
            game.update();
        }
    });
}
