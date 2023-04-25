#include <cubos/core/log.hpp>

#include <cubos/engine/assets/bridge.hpp>

using namespace cubos::engine;

bool AssetBridge::save(const Assets& /*assets*/, const AnyAsset& /*handle*/)
{
    CUBOS_ERROR("This asset bridge does not support saving assets");
    return false;
}
