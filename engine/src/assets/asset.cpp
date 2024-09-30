#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/string_view.hpp>
#include <cubos/core/reflection/external/uuid.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/tel/logging.hpp>

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
    : pathOrId(uuids::to_string(id))
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
        pathOrId = str;
        mId = id.value();
    }
    else if (str.find('/') != std::string_view::npos || str.find('\\') != std::string_view::npos)
    {
        pathOrId = str;
    }
    else
    {
        CUBOS_ERROR("Could not create asset handle, invalid UUID: \"{}\"", str);
    }
}

AnyAsset::AnyAsset(const AnyAsset& other)
    : pathOrId(other.pathOrId)
    , mId(other.mId)
    , mRefCount(other.mRefCount)
    , mVersion(other.mVersion)
{
    this->incRef();
}

AnyAsset::AnyAsset(AnyAsset&& other) noexcept
    : pathOrId(other.pathOrId)
    , mId(other.mId)
    , mRefCount(other.mRefCount)
    , mVersion(other.mVersion)
{
    other.mRefCount = nullptr;
}

AnyAsset& AnyAsset::operator=(const AnyAsset& other)
{
    if (this == &other)
    {
        return *this;
    }

    this->decRef();
    pathOrId = other.pathOrId;
    mId = other.mId;
    mRefCount = other.mRefCount;
    mVersion = other.mVersion;
    this->incRef();
    return *this;
}

AnyAsset& AnyAsset::operator=(AnyAsset&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    this->decRef();
    pathOrId = other.pathOrId;
    mId = other.mId;
    mRefCount = other.mRefCount;
    mVersion = other.mVersion;
    other.mRefCount = nullptr;
    return *this;
}

bool AnyAsset::operator==(const AnyAsset& other) const
{
    return this->getIdString() == other.getIdString();
}

int AnyAsset::getVersion() const
{
    return getId().has_value() && getId().value() == mId ? mVersion : 0;
}

std::string AnyAsset::getIdString() const
{
    return pathOrId;
}

std::optional<uuids::uuid> AnyAsset::getId() const
{
    return uuids::uuid::from_string(pathOrId);
}

bool AnyAsset::isNull() const
{
    return pathOrId.empty();
}

bool AnyAsset::isStrong() const
{
    return getId().has_value() && getId().value() == mId && mRefCount != nullptr;
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
        .with(FieldsTrait().withField("id", &AnyAsset::pathOrId));
}

void AnyAsset::incRef() const
{
    if (getId().has_value() && getId().value() == mId && mRefCount != nullptr)
    {
        static_cast<std::atomic<int>*>(mRefCount)->fetch_add(1);
    }
}

void AnyAsset::decRef() const
{
    if (getId().has_value() && getId().value() == mId && mRefCount != nullptr)
    {
        static_cast<std::atomic<int>*>(mRefCount)->fetch_sub(1);
    }
}
