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
    : idOrPath(uuids::to_string(id))
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
        idOrPath = str;
        mId = id.value();
    }
    else
    {
        if (str.find('/') != std::string::npos || str.find('\\') != std::string::npos)
        {
            idOrPath = str;
            path = str;
        }
        else
        {
            CUBOS_ERROR("Could not create asset handle, invalid UUID or Path: \"{}\"", str);
        }
    }
}

AnyAsset::AnyAsset(const AnyAsset& other)
    : idOrPath(other.idOrPath)
    , mId(other.mId)
    , mRefCount(other.mRefCount)
    , mVersion(other.mVersion)
    , path(other.path)
{
    this->incRef();
}

AnyAsset::AnyAsset(AnyAsset&& other) noexcept
    : idOrPath(other.idOrPath)
    , mId(other.mId)
    , mRefCount(other.mRefCount)
    , mVersion(other.mVersion)
    , path(other.path)
{
    other.mRefCount = nullptr;
}

AnyAsset& AnyAsset::operator=(const AnyAsset& other)
{
    this->decRef();
    idOrPath = other.idOrPath;
    mId = other.mId;
    mRefCount = other.mRefCount;
    mVersion = other.mVersion;
    path = other.path;
    this->incRef();
    return *this;
}

AnyAsset& AnyAsset::operator=(AnyAsset&& other) noexcept
{
    this->decRef();
    idOrPath = other.idOrPath;
    mId = other.mId;
    mRefCount = other.mRefCount;
    mVersion = other.mVersion;
    other.mRefCount = nullptr;
    path = other.path;
    return *this;
}

bool AnyAsset::operator==(const AnyAsset& other) const
{
    return this->getId() == other.getId();
}

int AnyAsset::getVersion() const
{
    return uuids::uuid::from_string(idOrPath) == mId ? mVersion : 0;
}

std::string AnyAsset::getId() const
{
    return idOrPath;
}

std::string AnyAsset::getPath() const
{
    return path;
}

bool AnyAsset::isNull() const
{
    return idOrPath.empty();
}

bool AnyAsset::isStrong() const
{
    return uuids::uuid::from_string(idOrPath) == mId && mRefCount != nullptr;
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
        .with(FieldsTrait().withField("id", &AnyAsset::idOrPath));
}

void AnyAsset::incRef() const
{
    if (uuids::uuid::from_string(idOrPath) == mId && mRefCount != nullptr)
    {
        static_cast<std::atomic<int>*>(mRefCount)->fetch_add(1);
    }
}

void AnyAsset::decRef() const
{
    if (uuids::uuid::from_string(idOrPath) == mId && mRefCount != nullptr)
    {
        static_cast<std::atomic<int>*>(mRefCount)->fetch_sub(1);
    }
}
