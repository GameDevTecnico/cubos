#include <cubos/engine/assets/meta.hpp>

#include <cubos/core/data/serializer.hpp>
#include <cubos/core/data/deserializer.hpp>

using namespace cubos::engine;

template <>
void cubos::core::data::serialize<AssetMeta>(Serializer& ser, const AssetMeta& meta, const char* name)
{
    // Create a copy of the parameters, so we can remove the excluded ones.
    std::unordered_map<std::string, std::string> params = meta.params();
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
void cubos::core::data::deserialize<AssetMeta>(Deserializer& des, AssetMeta& meta)
{
    des.read(meta.params());
}

std::optional<std::string> AssetMeta::get(const std::string& key) const
{
    auto it = this->m_params.find(key);
    if (it != this->m_params.end())
    {
        return it->second;
    }
    return std::nullopt;
}

void AssetMeta::set(const std::string& key, const std::string& value)
{
    this->m_params[key] = value;
}

void AssetMeta::remove(const std::string& key)
{
    this->m_params.erase(key);
}
