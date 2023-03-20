#include <cubos/engine/assets/asset.hpp>

#include <cubos/core/data/serializer.hpp>
#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/log.hpp>

using namespace cubos::engine;

template <>
void cubos::core::data::serialize<Asset>(Serializer& ser, const Asset& asset, const char* name)
{
    ser.write(uuids::to_string(asset.getId()), name);
}

template <>
void cubos::core::data::deserialize<Asset>(Deserializer& des, Asset& asset)
{
    std::string str;
    des.read(str);
    if (auto id = uuids::uuid::from_string(str))
    {
        asset = Asset(id.value());
    }
    else
    {
        CUBOS_ERROR("Could not deserialize asset handle, invalid UUID: \"{}\"", str);
        des.fail();
    }
}

inline static void incRef(void* refCount)
{
    if (refCount != nullptr)
    {
        static_cast<std::atomic<int>*>(refCount)->fetch_add(1);
    }
}

inline static void decRef(void* refCount)
{
    if (refCount != nullptr)
    {
        static_cast<std::atomic<int>*>(refCount)->fetch_sub(1);
    }
}

Asset::~Asset()
{
    decRef(this->refCount);
}

Asset::Asset(std::nullptr_t ptr) : id(), refCount(nullptr), version(-1)
{
}

Asset::Asset(uuids::uuid id) : id(id), refCount(nullptr), version(-1)
{
}

Asset::Asset(const Asset& other) : id(other.id), refCount(other.refCount), version(other.version)
{
    incRef(this->refCount);
}

Asset::Asset(Asset&& other) : id(other.id), refCount(other.refCount), version(other.version)
{
    other.refCount = nullptr;
}

Asset& Asset::operator=(const Asset& other)
{
    decRef(this->refCount);
    this->id = other.id;
    this->refCount = other.refCount;
    this->version = other.version;
    incRef(this->refCount);
    return *this;
}

Asset& Asset::operator=(Asset&& other)
{
    decRef(this->refCount);
    this->id = other.id;
    this->refCount = other.refCount;
    this->version = other.version;
    other.refCount = nullptr;
    return *this;
}

int Asset::getVersion() const
{
    return this->version;
}

uuids::uuid Asset::getId() const
{
    return this->id;
}

bool Asset::isNull() const
{
    return this->id.is_nil();
}

bool Asset::isStrong() const
{
    return this->refCount != nullptr;
}

void Asset::makeWeak()
{
    decRef(this->refCount);
    this->refCount = nullptr;
}
