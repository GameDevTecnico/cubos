#include <cubos/engine/assets/bridge.hpp>

#include <cubos/core/log.hpp>

using namespace cubos::engine;

bool AssetBridge::save(Assets& assets, Asset handle)
{
    CUBOS_ERROR("This asset bridge does not support saving assets");
    return false;
}
