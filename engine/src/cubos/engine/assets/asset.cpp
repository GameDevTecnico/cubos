#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/assets/asset.hpp>

using namespace cubos::engine;

template <>
void cubos::core::data::serialize<AnyAsset>(Serializer& ser, const AnyAsset& obj, const char* name)
{
    ser.write(uuids::to_string(obj.getId()), name);
}

template <>
void cubos::core::data::deserialize<AnyAsset>(Deserializer& des, AnyAsset& obj)
{
    std::string str;
    des.read(str);
    if (auto id = uuids::uuid::from_string(str))
    {
        obj = AnyAsset(id.value());
    }
    else
    {
        CUBOS_ERROR("Could not deserialize asset handle, invalid UUID: \"{}\"", str);
        des.fail();
    }
}

AnyAsset::~AnyAsset()
{
    this->decRef();
}

AnyAsset::AnyAsset(std::nullptr_t)
    : refCount(nullptr)
    , version(-1)
{
}

AnyAsset::AnyAsset(uuids::uuid id)
    : id(id)
    , refCount(nullptr)
    , version(-1)
{
}

AnyAsset::AnyAsset(std::string_view str)
    : refCount(nullptr)
    , version(-1)
{
    if (auto id = uuids::uuid::from_string(str))
    {
        this->id = id.value();
    }
    else
    {
        CUBOS_ERROR("Could not create asset handle, invalid UUID: \"{}\"", str);
    }
}

AnyAsset::AnyAsset(const AnyAsset& other)
    : id(other.id)
    , refCount(other.refCount)
    , version(other.version)
{
    this->incRef();
}

AnyAsset::AnyAsset(AnyAsset&& other)
    : id(other.id)
    , refCount(other.refCount)
    , version(other.version)
{
    other.refCount = nullptr;
}

AnyAsset& AnyAsset::operator=(const AnyAsset& other)
{
    this->decRef();
    this->id = other.id;
    this->refCount = other.refCount;
    this->version = other.version;
    this->incRef();
    return *this;
}

AnyAsset& AnyAsset::operator=(AnyAsset&& other)
{
    this->decRef();
    this->id = other.id;
    this->refCount = other.refCount;
    this->version = other.version;
    other.refCount = nullptr;
    return *this;
}

int AnyAsset::getVersion() const
{
    return this->version;
}

uuids::uuid AnyAsset::getId() const
{
    return this->id;
}

bool AnyAsset::isNull() const
{
    return this->id.is_nil();
}

bool AnyAsset::isStrong() const
{
    return this->refCount != nullptr;
}

void AnyAsset::makeWeak()
{
    this->decRef();
    this->refCount = nullptr;
}

void AnyAsset::incRef() const
{
    if (this->refCount != nullptr)
    {
        static_cast<std::atomic<int>*>(this->refCount)->fetch_add(1);
    }
}

void AnyAsset::decRef() const
{
    if (this->refCount != nullptr)
    {
        static_cast<std::atomic<int>*>(this->refCount)->fetch_sub(1);
    }
}
