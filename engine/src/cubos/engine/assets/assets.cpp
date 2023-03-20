#include <cubos/engine/assets/assets.hpp>

using namespace cubos::engine;

void Assets::loadMeta(std::string_view path)
{
    // TODO
}

void Assets::registerBridge(std::string extension, std::unique_ptr<AssetBridge> bridge)
{
    // TODO
}

const AssetMeta& Assets::meta(Asset handle) const
{
    // TODO
    abort();
}

AssetMeta& Assets::meta(Asset handle)
{
    // TODO
    abort();
}

Asset Assets::load(Asset handle)
{
    // TODO
    abort();
}

bool Assets::save(Asset handle, bool onlyMeta)
{
    // TODO
    return false;
}

Assets::Status Assets::status(Asset handle) const
{
    // TODO
    abort();
}

bool Assets::update(Asset& handle)
{
    // TODO
    return false;
}
