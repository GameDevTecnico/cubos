#include <memory>

#include <cubos/core/data/des/binary.hpp>
#include <cubos/core/data/ser/binary.hpp>
#include <cubos/core/memory/any_value.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/type_client.hpp>

#include "type_client_server_defaults.hpp"

using cubos::core::memory::AnyValue;
using cubos::core::memory::Opt;
using cubos::core::memory::Stream;
using cubos::core::reflection::TypeClient;
using cubos::core::reflection::TypeRegistry;

TypeClient::TypeClient()
{
    setupTypeClientDefaults(*this);
}

void TypeClient::addTrait(bool isStructural, const Type& traitType, Deserialize deserialize,
                          DiscoverTypes discoverTypes)
{
    mTraits.insert(traitType, {memory::move(deserialize), memory::move(discoverTypes), isStructural});
    auto& trait = mTraits.at(traitType);

    // Re-discover types that are already known and have the new trait.
    auto oldKnownTypes = mKnownTypes;
    for (auto [type, _] : oldKnownTypes)
    {
        if (type->has(traitType))
        {
            trait.discoverTypes(*type, [this](const Type& type) { this->addType(type); });
        }
    }
}

void TypeClient::addType(const Type& type)
{
    if (mKnownTypes.contains(type))
    {
        return;
    }

    CUBOS_ASSERT(type.has<ConstructibleTrait>(), "Known types added to type clients must be constructible");
    CUBOS_ASSERT(type.get<ConstructibleTrait>().hasDefaultConstruct(),
                 "Known types added to type clients must have a default constructor");

    // We insert it into the set immediately to avoid infinite recursion in case of circular dependencies.
    mKnownTypes.insert(type);

    // Add types which can be discovered from the new type.
    for (const auto& [traitType, trait] : mTraits)
    {
        if (type.has(*traitType))
        {
            trait.discoverTypes(type, [this](const Type& type) { this->addType(type); });
        }
    }
}

Opt<TypeClient::Connection> TypeClient::connect(Stream& stream) const
{
    // Send list of supported traits.
    data::BinarySerializer ser{stream};
    if (!ser.write(mTraits.size()))
    {
        CUBOS_ERROR("Could not serialize trait count");
        return {};
    }

    for (const auto& [traitType, trait] : mTraits)
    {
        if (!ser.write(traitType->name()) || !ser.write(trait.isStructural))
        {
            CUBOS_ERROR("Could not serialize trait name or structural flag");
            return {};
        }
    }

    // Send list of already known types, so that the server doesn't have to send them.
    if (!ser.write(mKnownTypes.size()))
    {
        CUBOS_ERROR("Could not serialize known type count");
        return {};
    }

    TypeRegistry types;
    for (const auto& [type, _] : mKnownTypes)
    {
        if (!ser.write(type->name()))
        {
            CUBOS_ERROR("Could not serialize known type name");
            return {};
        }

        types.insert(*type);
    }

    // Read unknown type information data from the stream.
    data::BinaryDeserializer des{stream};
    std::size_t typeCount;
    if (!des.read(typeCount))
    {
        CUBOS_ERROR("Could not deserialize type count");
        return {};
    }

    // Iterate over each received type.
    TypeRegistry serializableTypes;
    std::vector<Type*> externalTypes;
    for (std::size_t i = 0; i < typeCount; ++i)
    {
        // Read type name.
        std::string typeName;
        if (!des.read(typeName))
        {
            CUBOS_ERROR("Could not deserialize type name");
            return {};
        }

        // There should be no duplicate type names.
        if (mKnownTypes.contains(typeName))
        {
            CUBOS_ERROR("Received duplicate type name {}", typeName);
            return {};
        }

        // Read type trait count.
        std::size_t traitCount;
        if (!des.read(traitCount))
        {
            CUBOS_ERROR("Could not deserialize trait count for type {}", typeName);
            return {};
        }

        // Create a type with the given name.
        auto& type = Type::create(typeName);

        // Read type traits.
        for (std::size_t j = 0; j < traitCount; ++j)
        {
            // Read trait name.
            std::string traitName;
            if (!des.read(traitName))
            {
                CUBOS_ERROR("Could not deserialize trait name");
                return {};
            }

            // Find trait type.
            const Type* traitType = nullptr;
            for (const auto& [type, _] : mTraits)
            {
                if (type->name() == traitName)
                {
                    traitType = type;
                    break;
                }
            }

            if (traitType == nullptr)
            {
                CUBOS_ERROR("Server sent non-agreed upon trait {}", traitName);
                return {};
            }

            // Check if the trait is structural.
            const auto& trait = mTraits.at(*traitType);
            if (trait.isStructural && j != 0)
            {
                // Structural traits must be the first ones to be received!
                CUBOS_ERROR("Received a structural trait {} for type {} but it was not the first trait", traitName,
                            typeName);
                return {};
            }

            // Deserialize trait data into the type.
            bool hadRemoteTrait = type.has<RemoteTrait>();
            if (!trait.deserialize(types, type, des))
            {
                CUBOS_ERROR("Could not deserialize trait {} for type {}", traitName, typeName);
                return {};
            }

            if (trait.isStructural)
            {
                // If the trait that was just added is structural, then it must have added a RemoteTrait to the type.
                if (!type.has<RemoteTrait>())
                {
                    CUBOS_ERROR("Received structural trait {} for type {}, but it did not add a RemoteTrait", traitName,
                                typeName);
                    return {};
                }

                // It must also have added a constructible trait.
                if (!type.get<RemoteTrait>().constructibleTrait.contains())
                {
                    CUBOS_ERROR("Received structural trait {} for type {}, but the RemoteTrait did not contain a "
                                "ConstructibleTrait",
                                traitName, typeName);
                    return {};
                }
            }
            else
            {
                CUBOS_ASSERT_IMP(!hadRemoteTrait, !type.has<RemoteTrait>(),
                                 "Non-structural trait {} added a RemoteTrait to the type, which is not allowed",
                                 traitName);
            }
        }

        // Check if the type has a default constructor.
        bool hasDefaultConstruct;
        if (!des.read(hasDefaultConstruct))
        {
            CUBOS_ERROR("Could not deserialize default construct flag for type {}", typeName);
            return {};
        }

        if (type.has<RemoteTrait>())
        {
            serializableTypes.insert(type);

            // If the type has a RemoteTrait, then we must add a ConstructibleTrait to the type.
            const auto& remoteTrait = type.get<RemoteTrait>();
            const auto& remoteConstructibleTrait = remoteTrait.constructibleTrait.value();

            // If the server is sending us a default constructor, then we'll deserialize the default value, and add a
            // ConstructibleTrait to the type which uses it.
            if (hasDefaultConstruct)
            {
                // First initialize an instance of the type.
                void* defaultValueRaw = operator new(
                    remoteConstructibleTrait.size(),
                    static_cast<std::align_val_t>(remoteConstructibleTrait.alignment()));
                remoteConstructibleTrait.defaultConstruct(defaultValueRaw);

                // Then deserialize the default value.
                if (!des.read(type, defaultValueRaw))
                {
                    CUBOS_ERROR("Could not deserialize default value of type {} with traits {}", typeName,
                                this->listTraits(type));
                    return {};
                }

                // Wrap the default value in a unique pointer to ensure it gets destroyed when the type is destroyed.
                std::unique_ptr<void, memory::Function<void(void*)>> defaultValue{
                    defaultValueRaw, remoteConstructibleTrait.destructor()};

                // Add a constructible trait to the type which uses the default value above.
                type.with(ConstructibleTrait{remoteConstructibleTrait.size(), remoteConstructibleTrait.alignment(),
                                             remoteConstructibleTrait.destructor()}
                              .withDefaultConstructor([defaultValue = memory::move(defaultValue),
                                                       copyConstructor = remoteConstructibleTrait.copyConstructor()](
                                                          void* value) { copyConstructor(value, defaultValue.get()); })
                              .withCopyConstructor(remoteConstructibleTrait.copyConstructor())
                              .withMoveConstructor(remoteConstructibleTrait.moveConstructor()));
            }
            else
            {
                // Otherwise, we'll add our own default constructor to the type.
                type.with(ConstructibleTrait{remoteConstructibleTrait.size(), remoteConstructibleTrait.alignment(),
                                             remoteConstructibleTrait.destructor()}
                              .withDefaultConstructor(remoteConstructibleTrait.defaultConstructor())
                              .withCopyConstructor(remoteConstructibleTrait.copyConstructor())
                              .withMoveConstructor(remoteConstructibleTrait.moveConstructor()));
            }

            CUBOS_TRACE("Received serializable type {} with traits {}", typeName, this->listTraits(type));
        }
        else if (hasDefaultConstruct)
        {
            // For the type to have a default constructor, it must have had a structural trait.
            CUBOS_ERROR(
                "Received type {} with default constructor from server although it doesn't have a structural trait",
                typeName);
            return {};
        }
        else
        {
            type.with(RemoteTrait{});
            CUBOS_TRACE("Received non-serializable type {} with traits {}", typeName, this->listTraits(type));
        }

        // Add the new type to the registry and list of external types.
        types.insert(type);
        externalTypes.push_back(&type);
    }

    return Connection{memory::move(types), memory::move(serializableTypes), memory::move(externalTypes)};
}

std::string TypeClient::listTraits(const Type& type) const
{
    std::string traits;

    for (const auto& [trait, _] : mTraits)
    {
        if (type.has(*trait))
        {
            if (!traits.empty())
            {
                traits += ", ";
            }
            traits += trait->name();
        }
    }

    return traits;
}

TypeClient::Connection::~Connection()
{
    // We destroy the external types from back to front, as the ones in the back might depend on the ones in the front.
    for (std::size_t i = mExternalTypes.size(); i > 0; --i)
    {
        Type::destroy(*mExternalTypes[i - 1]);
    }
}

TypeClient::Connection::Connection(TypeRegistry sharedTypes, TypeRegistry serializableTypes,
                                   std::vector<Type*> externalTypes)
    : mSharedTypes{memory::move(sharedTypes)}
    , mSerializableTypes{memory::move(serializableTypes)}
    , mExternalTypes{memory::move(externalTypes)}
{
}

TypeClient::Connection::Connection(Connection&& other) noexcept
    : mSharedTypes(memory::move(other.mSharedTypes))
    , mSerializableTypes(memory::move(other.mSerializableTypes))
    , mExternalTypes(memory::move(other.mExternalTypes))
{
    other.mExternalTypes.clear();
}

const TypeRegistry& TypeClient::Connection::sharedTypes() const
{
    return mSharedTypes;
}

const TypeRegistry& TypeClient::Connection::serializableTypes() const
{
    return mSerializableTypes;
}

CUBOS_REFLECT_IMPL(TypeClient::RemoteTrait)
{
    return Type::create("cubos::core::reflection::TypeClient::RemoteTrait");
}
