#include <cubos/engine/assets/meta.hpp>

using namespace cubos::engine;

std::optional<std::string> AssetMeta::get(std::string_view key) const
{
    auto it = mParams.find(std::string(key));
    if (it != mParams.end())
    {
        return it->second;
    }
    return std::nullopt;
}

void AssetMeta::set(std::string_view key, std::string_view value)
{
    mParams[std::string(key)] = value;
}

void AssetMeta::remove(std::string_view key)
{
    mParams.erase(std::string(key));
}

uuids::uuid AssetMeta::getId() const
{
    return uuids::uuid::from_string(this->get("id").value()).value();
}

std::optional<std::string> AssetMeta::getPath() const
{
    return this->get("path");
}
