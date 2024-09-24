/// @file
/// @brief Class @ref cubos::core::reflection::TypeServer.
/// @ingroup core-reflection

#pragma once

#include <vector>

#include <cubos/core/data/ser/serializer.hpp>
#include <cubos/core/memory/function.hpp>
#include <cubos/core/memory/opt.hpp>
#include <cubos/core/memory/type_map.hpp>
#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/type_registry.hpp>

namespace cubos::core::reflection
{
    /// @brief Used to setup a @ref TypeChannel on top of a stream, from the server side.
    ///
    /// This class allows the server to send and receive values of types unknown by the client to the client.
    /// Not all types registered on the server will support this: some may not have enough information to be serialized.
    /// The set of types which can be safely serialized is returned by the @ref connect function.
    ///
    /// The protocol used by this class is as follows:
    /// 1. Server receives a list of supported trait types from the client, and whether they are structural.
    /// 2. Server receives a list of the types which the client already knows.
    /// 3. Server sends the type information of the types which the client does not know. This includes:
    ///   a) name of each type,
    ///   b) structural trait of the type which is supported by the client, if any,
    ///   b) non-structural traits of the type which are supported by the client,
    ///   d) if the type has a structural trait and is default-constructible, the default-constructed serialized value.
    ///
    /// If the type has multiple structural traits, the first one added to the server is used.
    /// The non-structural traits are sent in the order they were added to the server.
    ///
    /// @ingroup core-reflection
    class CUBOS_CORE_API TypeServer final
    {
    public:
        /// @brief Class which holds connection information such as the type registry.
        class Connection;

        /// @brief Function type for serializing traits.
        ///
        /// Receives a serializer, the value to serialize, and whether the type is already serializable.
        using Serialize = memory::Function<bool(data::Serializer&, const Type&, bool) const>;

        /// @brief Function type for discovering new types from a trait.
        using DiscoverTypes = void (*)(const Type&, memory::Function<void(const Type&) const>&);

        /// @brief Default constructs.
        TypeServer();

        /// @brief Registers a trait to be supported by the channel.
        /// @note If a connection is already active, will only take effect after the channel is reconnected.
        /// @param type Trait type to be registered.
        /// @param serialize Function to serialize the trait.
        /// @param discoverTypes Function to discover new types from the trait.
        void addTrait(const Type& traitType, Serialize serialize, DiscoverTypes discoverTypes);

        /// @brief Registers a trait to be supported by the channel.
        /// @note If a connection is already active, will only take effect after the channel is reconnected.
        /// @tparam T Trait type.
        /// @param serialize Function to serialize the trait.
        /// @param discoverTypes Function to discover new types from the trait.
        template <typename T>
        void addTrait(Serialize serialize, DiscoverTypes discoverTypes)
        {
            this->addTrait(reflect<T>(), memory::move(serialize), memory::move(discoverTypes));
        }

        /// @brief Adds a type to be supported by the channel.
        ///
        /// Also adds any types used by the type, recursively. I.e., types of fields if the type has a @ref FieldsTrait.
        ///
        /// @param type Type to be supported.
        void addType(const Type& type);

        /// @brief Adds a type to be supported by the channel.
        ///
        /// Also adds any types used by the type, recursively. I.e., types of fields if the type has a @ref FieldsTrait.
        ///
        /// @tparam T Type to be supported.
        template <typename T>
        void addType()
        {
            this->addType(reflect<T>());
        }

        /// @brief Connects to a new client, using the given stream.
        /// @param stream Stream.
        /// @return Connection information, or nothing if the connection failed.
        memory::Opt<Connection> connect(memory::Stream& stream);

    private:
        struct Trait
        {
            Serialize serialize;
            DiscoverTypes discoverTypes;
        };

        TypeRegistry mKnownTypes;
        std::vector<const Type*> mOrderedKnownTypes;
        memory::TypeMap<Trait> mTraits;
        std::vector<const Type*> mOrderedTraits; // Preserve order of insertion.
    };

    class TypeServer::Connection
    {
    public:
        /// @brief Constructs.
        /// @param sharedTypes All types which are shared between the server and the client.
        /// @param serializableTypes All types which are serializable by the client.
        Connection(TypeRegistry sharedTypes, TypeRegistry serializableTypes);

        /// @brief Move constructs.
        /// @param other Other.
        Connection(Connection&& other) noexcept = default;

        /// @brief Gets the shared types.
        /// @return Shared types.
        const TypeRegistry& sharedTypes() const;

        /// @brief Gets the serializable types.
        /// @return Serializable types.
        const TypeRegistry& serializableTypes() const;

    private:
        TypeRegistry mSharedTypes;
        TypeRegistry mSerializableTypes;
    };
} // namespace cubos::core::reflection
