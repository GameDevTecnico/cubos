#include <cubos/core/data/old/deserializer.hpp>
#include <cubos/core/data/old/serializer.hpp>

#include <cubos/engine/assets/meta.hpp>

using namespace cubos::engine;

template <>
void cubos::core::data::old::serialize<AssetMeta>(Serializer& ser, const AssetMeta& obj, const char* name)
{
    // Create a copy of the parameters, so we can remove the excluded ones.
    std::unordered_map<std::string, std::string> params = obj.params();
    if (ser.context().has<AssetMeta::Exclude>())
    {
        const auto& exclude = ser.context().get<AssetMeta::Exclude>();
        for (const auto& key : exclude.keys)
        {
            params.erase(key);
        }
    }

    ser.write(params, name);
}

template <>
void cubos::core::data::old::deserialize<AssetMeta>(Deserializer& des, AssetMeta& obj)
{
    des.read(obj.params());
}

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
