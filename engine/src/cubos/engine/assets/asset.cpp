#include <cubos/core/data/old/deserializer.hpp>
#include <cubos/core/data/old/serializer.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/string_view.hpp>
#include <cubos/core/reflection/external/uuid.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/assets/asset.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(AnyAsset)
{
    return AnyAsset::makeType("cubos::engine::AnyAsset");
}

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
    : reflectedId(id)
    , mId(id)
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
        reflectedId = id.value();
        mId = id.value();
    }
    else
    {
        CUBOS_ERROR("Could not create asset handle, invalid UUID: \"{}\"", str);
    }
}

AnyAsset::AnyAsset(const AnyAsset& other)
    : reflectedId(other.reflectedId)
    , mId(other.mId)
    , mRefCount(other.mRefCount)
    , mVersion(other.mVersion)
{
    this->incRef();
}

AnyAsset::AnyAsset(AnyAsset&& other) noexcept
    : reflectedId(other.reflectedId)
    , mId(other.mId)
    , mRefCount(other.mRefCount)
    , mVersion(other.mVersion)
{
    other.mRefCount = nullptr;
}

AnyAsset& AnyAsset::operator=(const AnyAsset& other)
{
    this->decRef();
    reflectedId = other.reflectedId;
    mId = other.mId;
    mRefCount = other.mRefCount;
    mVersion = other.mVersion;
    this->incRef();
    return *this;
}

AnyAsset& AnyAsset::operator=(AnyAsset&& other) noexcept
{
    this->decRef();
    reflectedId = other.reflectedId;
    mId = other.mId;
    mRefCount = other.mRefCount;
    mVersion = other.mVersion;
    other.mRefCount = nullptr;
    return *this;
}

bool AnyAsset::operator==(const AnyAsset& other) const
{
    return this->getId() == other.getId();
}

int AnyAsset::getVersion() const
{
    return reflectedId == mId ? mVersion : 0;
}

uuids::uuid AnyAsset::getId() const
{
    return reflectedId;
}

bool AnyAsset::isNull() const
{
    return reflectedId.is_nil();
}

bool AnyAsset::isStrong() const
{
    return reflectedId == mId && mRefCount != nullptr;
}

void AnyAsset::makeWeak()
{
    this->decRef();
    mRefCount = nullptr;
}

cubos::core::reflection::Type& AnyAsset::makeType(std::string name)
{
    using namespace cubos::core::reflection;

    return Type::create(std::move(name))
        .with(ConstructibleTrait::typed<AnyAsset>().withBasicConstructors().build())
        .with(FieldsTrait().withField("id", &AnyAsset::reflectedId));
}

void AnyAsset::incRef() const
{
    if (reflectedId == mId && mRefCount != nullptr)
    {
        static_cast<std::atomic<int>*>(mRefCount)->fetch_add(1);
    }
}

void AnyAsset::decRef() const
{
    if (reflectedId == mId && mRefCount != nullptr)
    {
        static_cast<std::atomic<int>*>(mRefCount)->fetch_sub(1);
    }
}

void AnyAsset::serialize(core::data::old::Serializer& ser, const char* name) const
{
    ser.write(uuids::to_string(this->getId()), name);
}

void AnyAsset::deserialize(core::data::old::Deserializer& des)
{
    std::string str;
    des.read(str);
    if (auto id = uuids::uuid::from_string(str))
    {
        this->decRef();
        reflectedId = id.value();
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
