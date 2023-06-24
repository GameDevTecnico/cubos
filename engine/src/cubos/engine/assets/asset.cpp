#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/assets/asset.hpp>

using namespace cubos::engine;

AnyAsset::~AnyAsset()
{
    this->decRef();
}

AnyAsset::AnyAsset(std::nullptr_t)
    : mRefCount(nullptr)
    , mVersion(-1)
{
}

AnyAsset::AnyAsset(uuids::uuid id)
    : mId(id)
    , mRefCount(nullptr)
    , mVersion(-1)
{
}

AnyAsset::AnyAsset(std::string_view str)
    : mRefCount(nullptr)
    , mVersion(-1)
{
    if (auto id = uuids::uuid::from_string(str))
    {
        mId = id.value();
    }
    else
    {
        CUBOS_ERROR("Could not create asset handle, invalid UUID: \"{}\"", str);
    }
}

AnyAsset::AnyAsset(const AnyAsset& other)
    : mId(other.mId)
    , mRefCount(other.mRefCount)
    , mVersion(other.mVersion)
{
    this->incRef();
}

AnyAsset::AnyAsset(AnyAsset&& other) noexcept
    : mId(other.mId)
    , mRefCount(other.mRefCount)
    , mVersion(other.mVersion)
{
    other.mRefCount = nullptr;
}

AnyAsset& AnyAsset::operator=(const AnyAsset& other)
{
    this->decRef();
    mId = other.mId;
    mRefCount = other.mRefCount;
    mVersion = other.mVersion;
    this->incRef();
    return *this;
}

AnyAsset& AnyAsset::operator=(AnyAsset&& other) noexcept
{
    this->decRef();
    mId = other.mId;
    mRefCount = other.mRefCount;
    mVersion = other.mVersion;
    other.mRefCount = nullptr;
    return *this;
}

int AnyAsset::getVersion() const
{
    return mVersion;
}

uuids::uuid AnyAsset::getId() const
{
    return mId;
}

bool AnyAsset::isNull() const
{
    return mId.is_nil();
}

bool AnyAsset::isStrong() const
{
    return mRefCount != nullptr;
}

void AnyAsset::makeWeak()
{
    this->decRef();
    mRefCount = nullptr;
}

void AnyAsset::incRef() const
{
    if (mRefCount != nullptr)
    {
        static_cast<std::atomic<int>*>(mRefCount)->fetch_add(1);
    }
}

void AnyAsset::decRef() const
{
    if (mRefCount != nullptr)
    {
        static_cast<std::atomic<int>*>(mRefCount)->fetch_sub(1);
    }
}

void AnyAsset::serialize(core::old::data::Serializer& ser, const char* name) const
{
    ser.write(uuids::to_string(this->getId()), name);
}

void AnyAsset::deserialize(core::old::data::Deserializer& des)
{
    std::string str;
    des.read(str);
    if (auto id = uuids::uuid::from_string(str))
    {
        this->decRef();
        mId = id.value();
        mRefCount = nullptr;
        mVersion = -1;
    }
    else
    {
        CUBOS_ERROR("Could not deserialize asset handle, invalid UUID: \"{}\"", str);
        des.fail();
    }
}
