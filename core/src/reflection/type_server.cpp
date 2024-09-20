#include <unordered_set>

#include <cubos/core/data/des/binary.hpp>
#include <cubos/core/data/ser/binary.hpp>
#include <cubos/core/memory/any_value.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/reflection/type_server.hpp>

#include "type_client_server_defaults.hpp"

using cubos::core::memory::AnyValue;
using cubos::core::memory::Opt;
using cubos::core::memory::TypeMap;
using cubos::core::reflection::TypeRegistry;
using cubos::core::reflection::TypeServer;

TypeServer::TypeServer()
{
    setupTypeServerDefaults(*this);
}

void TypeServer::addTrait(const Type& traitType, Serialize serialize, DiscoverTypes discoverTypes)
{
    mTraits.insert(traitType, {memory::move(serialize), memory::move(discoverTypes)});
    mOrderedTraits.push_back(&traitType);
    const auto& trait = mTraits.at(traitType);

    // Re-discover types for all known types that have the new trait.
    auto oldKnownTypes = mKnownTypes;
    for (auto [type, _] : oldKnownTypes)
    {
        if (type->has(traitType))
        {
            trait.discoverTypes(*type, [this](const Type& type) { this->addType(type); });
        }
    }
}

void TypeServer::addType(const Type& type)
{
    if (mKnownTypes.contains(type))
    {
        return;
    }

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

    // Add the type to the list of known types, only after all its dependencies have been added.
    mOrderedKnownTypes.push_back(&type);
}

Opt<TypeServer::Connection> TypeServer::connect(memory::Stream& stream)
{
    // Start by receiving the set of supported traits.
    data::BinaryDeserializer des{stream};
    std::size_t traitCount;
    if (!des.read(traitCount))
    {
        CUBOS_ERROR("Could not deserialize trait count");
        return {};
    }

    TypeRegistry supportedTraits{};
    TypeRegistry supportedStructuralTraits{};

    for (std::size_t i = 0; i < traitCount; ++i)
    {
        std::string traitName;
        if (!des.read(traitName))
        {
            CUBOS_ERROR("Could not deserialize trait name");
            return {};
        }

        bool isStructural;
        if (!des.read(isStructural))
        {
            CUBOS_ERROR("Could not deserialize trait structural flag");
            return {};
        }

        // Check if we also support the trait.
        for (const auto& [type, _] : mTraits)
        {
            if (type->name() == traitName)
            {
                // If so, add it to the set of supported traits.
                supportedTraits.insert(*type);

                if (isStructural)
                {
                    CUBOS_TRACE("Client supports structural trait {}", type->name());
                    supportedStructuralTraits.insert(*type);
                }
                else
                {
                    CUBOS_TRACE("Client supports non-structural trait {}", type->name());
                }

                break;
            }
        }
    }

    // Log traits which we support but the client does not.
    for (const auto& [type, _] : mTraits)
    {
        if (!supportedTraits.contains(*type))
        {
            CUBOS_WARN("Client does not support trait {}", type->name());
        }
    }

    // Receive the already known type set.
    std::size_t knownTypeCount;
    if (!des.read(knownTypeCount))
    {
        CUBOS_ERROR("Could not deserialize known type count");
        return {};
    }

    std::unordered_set<std::string> knownTypeNames{};
    for (std::size_t i = 0; i < knownTypeCount; ++i)
    {
        std::string typeName;
        if (!des.read(typeName))
        {
            CUBOS_ERROR("Could not deserialize type name");
            return {};
        }

        knownTypeNames.emplace(std::move(typeName));
    }

    // Keep track of types which have a structural trait, or can be serialized.
    TypeRegistry structuredTypes{};

    // We add all of the primitive types of the binary serializer here, as they are always supported.
    structuredTypes.insert<bool>();
    structuredTypes.insert<char>();
    structuredTypes.insert<signed char>();
    structuredTypes.insert<short>();
    structuredTypes.insert<int>();
    structuredTypes.insert<long>();
    structuredTypes.insert<long long>();
    structuredTypes.insert<unsigned char>();
    structuredTypes.insert<unsigned short>();
    structuredTypes.insert<unsigned int>();
    structuredTypes.insert<unsigned long>();
    structuredTypes.insert<unsigned long long>();
    structuredTypes.insert<float>();
    structuredTypes.insert<double>();
    structuredTypes.insert<std::string>();

    // Send type information data to the stream.
    std::size_t sharedTypeCount = 0;
    for (const auto& [type, _] : mKnownTypes)
    {
        if (!knownTypeNames.contains(type->name()))
        {
            ++sharedTypeCount;
        }
    }

    data::BinarySerializer ser{stream};
    if (!ser.write(sharedTypeCount))
    {
        CUBOS_ERROR("Could not serialize type count");
        return {};
    }

    // Iterate over all known types, in the order they were added.
    for (const Type* type : mOrderedKnownTypes)
    {
        if (knownTypeNames.contains(type->name()))
        {
            // If the client already knows the type, skip it.
            CUBOS_TRACE("Skipping type {} as it is already known by the client", type->name());
            continue;
        }
        else
        {
            CUBOS_TRACE("Sharing type {}", type->name());
        }

        if (!ser.write(type->name()))
        {
            CUBOS_ERROR("Could not serialize type name");
            return {};
        }

        // Get the first structural trait of the type, if there's any.
        const Type* structuralTraitType = nullptr;
        for (const auto& trait : mOrderedTraits)
        {
            if (type->has(*trait) && supportedStructuralTraits.contains(*trait))
            {
                structuralTraitType = trait;
                break;
            }
        }

        // If we found a structural trait, make sure the types that are part of it are also structured.
        // If not, we pretend the type doesn't have a structural trait.
        if (structuralTraitType != nullptr)
        {
            auto& trait = mTraits.at(*structuralTraitType);

            trait.discoverTypes(*type, [&](const Type& discoveredType) {
                if (structuralTraitType != nullptr && !structuredTypes.contains(discoveredType))
                {
                    CUBOS_WARN("Type {} discovered from type {} through structural trait {} isn't structured, causing "
                               "{} to also not be structured",
                               discoveredType.name(), type->name(), structuralTraitType->name(), type->name());
                    structuralTraitType = nullptr;
                }
            });
        }

        // Count how many traits we'll be sending.
        traitCount = structuralTraitType != nullptr ? 1 : 0;
        for (const auto& traitType : mOrderedTraits)
        {
            // Count only non-structural traits of the type that are also supported by the client.
            if (type->has(*traitType) && supportedTraits.contains(*traitType) &&
                !supportedStructuralTraits.contains(*traitType))
            {
                ++traitCount;
            }
        }

        if (!ser.write(traitCount))
        {
            CUBOS_ERROR("Could not serialize trait count for type {}", type->name());
            return {};
        }

        if (structuralTraitType != nullptr)
        {
            // Share the structural trait with the client.
            structuredTypes.insert(*type);
            auto& trait = mTraits.at(*structuralTraitType);
            if (!ser.write(structuralTraitType->name()))
            {
                CUBOS_ERROR("Could not serialize structural trait name for type {}", type->name());
                return {};
            }

            if (!trait.serialize(ser, *type, false))
            {
                CUBOS_ERROR("Could not serialize structural trait for type {}", type->name());
                return {};
            }
        }
        else
        {
            CUBOS_WARN(
                "Type {} is not structured (no valid structural trait found), sharing only non-structural traits",
                type->name());
        }

        for (const auto& traitType : mOrderedTraits)
        {
            // Serialize only non-structural traits of the type that are also supported by the client.
            const auto& trait = mTraits.at(*traitType);
            if (type->has(*traitType) && supportedTraits.contains(*traitType) &&
                !supportedStructuralTraits.contains(*traitType))
            {
                if (!ser.write(traitType->name()))
                {
                    CUBOS_ERROR("Could not serialize trait name {} for type {}", traitType->name(), type->name());
                    return {};
                }

                if (!trait.serialize(ser, *type, structuralTraitType != nullptr))
                {
                    CUBOS_ERROR("Could not serialize trait {} for type {}", traitType->name(), type->name());
                    return {};
                }
            }
        }

        bool hasDefaultConstruct = structuralTraitType != nullptr && type->has<ConstructibleTrait>() &&
                                   type->get<ConstructibleTrait>().hasDefaultConstruct();
        if (!ser.write(hasDefaultConstruct))
        {
            CUBOS_ERROR("Could not serialize default construct flag for type {}", type->name());
            return {};
        }

        if (hasDefaultConstruct)
        {
            auto defaultValue = AnyValue::defaultConstruct(*type);
            if (!ser.write(defaultValue.type(), defaultValue.get()))
            {
                CUBOS_ERROR("Could not serialize default value for type {}", type->name());
                return {};
            }
        }
    }

    return Connection{mKnownTypes, memory::move(structuredTypes)};
}

TypeServer::Connection::Connection(TypeRegistry sharedTypes, TypeRegistry serializableTypes)
    : mSharedTypes{memory::move(sharedTypes)}
    , mSerializableTypes{memory::move(serializableTypes)}
{
}

const TypeRegistry& TypeServer::Connection::sharedTypes() const
{
    return mSharedTypes;
}

const TypeRegistry& TypeServer::Connection::serializableTypes() const
{
    return mSerializableTypes;
}
