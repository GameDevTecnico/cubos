/// @file
/// @brief Class @ref cubos::core::reflection::TypeClient.
/// @ingroup core-reflection

#pragma once

#include <cubos/core/data/des/deserializer.hpp>
#include <cubos/core/memory/any_value.hpp>
#include <cubos/core/memory/function.hpp>
#include <cubos/core/memory/opt.hpp>
#include <cubos/core/memory/type_map.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type_registry.hpp>

namespace cubos::core::reflection
{
    /// @brief Manages the client-side of a reflection channel on top of a stream.
    ///
    /// This class allows the client to receive type information of types unknown by the client from the server.
    /// It also makes it possible to then receive and send values of these types.
    ///
    /// Received types can be either structured or non-structured.
    /// Only values of structured types can be communicated between the client and the server.
    /// For a type to be structured, it must have a structural trait, so that the client can create new instances of the
    /// type.
    ///
    /// Structural traits are traits which define the memory layout of a type.
    /// A structural trait should add the @ref TypeClient::RemoteTrait trait to the type.
    /// This trait is then used by this class to create a @ref ConstructibleTrait for the type.
    ///
    /// See @ref TypeServer for more information on the protocol.
    ///
    /// @ingroup core-reflection
    class CUBOS_CORE_API TypeClient final
    {
    public:
        /// @brief Class which holds connection information such as the type registry.
        class Connection;

        /// @brief Trait added to all types received by the client.
        ///
        /// Structural traits should add this trait to their types when deserialized by the client.
        /// They should also initialize the @ref ConstructibleTrait field to the correct value.
        /// See @ref setupTypeClientDefaults for examples on how to register structural traits.
        ///
        /// If a type is not constructible/does not have a structural trait, this trait is added by the client itself,
        /// and the @ref ConstructibleTrait field is left empty.
        struct RemoteTrait;

        /// @brief Function type for deserializing traits.
        using Deserialize = memory::Function<bool(const TypeRegistry&, Type&, data::Deserializer&) const>;

        /// @brief Function type for discovering new types from a trait.
        using DiscoverTypes = void (*)(const Type&, memory::Function<void(const Type&) const>);

        /// @brief Default constructor.
        TypeClient();

        /// @brief Registers a trait to be supported by the channel.
        /// @param isStructural Whether the trait is a structural trait.
        /// @param traitType Trait type to be registered.
        /// @param deserialize Function to deserialize the trait.
        /// @param discoverTypes Function to discover new types from the trait.
        void addTrait(bool isStructural, const Type& traitType, Deserialize deserialize, DiscoverTypes discoverTypes);

        /// @brief Registers a trait to be supported by the channel.
        /// @tparam T Trait type.
        /// @param isStructural Whether the trait is a structural trait.
        /// @param deserialize Function to deserialize the trait.
        /// @param discoverTypes Function to discover new types from the trait.
        template <typename T>
        void addTrait(bool isStructural, Deserialize deserialize, DiscoverTypes discoverTypes)
        {
            this->addTrait(isStructural, reflect<T>(), memory::move(deserialize), memory::move(discoverTypes));
        }

        /// @brief Adds a known type to the channel.
        ///
        /// This allows the channel to reuse the given type instead of creating a new runtime type when connecting to a
        /// server.
        ///
        /// @note The type must be default constructible.
        /// @param type Type.
        void addType(const Type& type);

        /// @brief Adds a known type to the channel.
        ///
        /// This allows the channel to reuse the given type instead of creating a new runtime type when connecting to a
        /// server.
        ///
        /// @note The type must be default constructible.
        /// @tparam T Type.
        template <typename T>
        void addType()
        {
            this->addType(reflect<T>());
        }

        /// @brief Connects to a server, using the given stream.
        /// @param stream Stream.
        /// @return Connection information, or nothing if the connection failed.
        memory::Opt<Connection> connect(memory::Stream& stream) const;

    private:
        struct Trait
        {
            Deserialize deserialize;
            DiscoverTypes discoverTypes;
            bool isStructural;
        };

        /// @brief Lists the known traits of the given type.
        /// @param type Type.
        std::string listTraits(const Type& type) const;

        memory::TypeMap<Trait> mTraits;
        TypeRegistry mKnownTypes;
    };

    class TypeClient::Connection
    {
    public:
        ~Connection();

        /// @brief Constructs.
        /// @param types Type registry.
        /// @param externalTypes Types to be destroyed when the connection is destroyed.
        Connection(TypeRegistry sharedTypes, TypeRegistry serializableTypes, std::vector<Type*> externalTypes);

        /// @brief Move constructs.
        /// @param other Other.
        Connection(Connection&& other) noexcept;

        /// @brief Gets the type registry.
        /// @return Type registry.
        const TypeRegistry& sharedTypes() const;

        /// @brief Gets the type registry.
        /// @return Type registry.
        const TypeRegistry& serializableTypes() const;

    private:
        TypeRegistry mSharedTypes;
        TypeRegistry mSerializableTypes;
        std::vector<Type*> mExternalTypes;
    };

    struct TypeClient::RemoteTrait
    {
        CUBOS_REFLECT;

        /// @brief Constructible trait which is used to create raw values for instances of the type.
        ///
        /// Values default constructed by this trait do not have the same value as the actual default constructor of the
        /// host type. Instead, the @ref TypeClient creates a new @ref ConstructibleTrait for the type, from this one,
        /// whose default constructor correctly initializes the type to the default value received from the server.
        ///
        /// If the type has no structural trait, this field is left empty.
        /// Structural traits must initialize this field to the correct value.
        memory::Opt<ConstructibleTrait> constructibleTrait;

        /// @brief Automatically constructs a new RemoteTrait for the given type, with all basic constructors.
        /// @tparam T Type.
        template <typename T>
        static RemoteTrait createBasic()
        {
            return RemoteTrait{ConstructibleTrait::typed<T>().withBasicConstructors().build()};
        }

        /// @brief Automatically constructs a new RemoteTrait for the given type, with the copy and move constructors,
        /// and a custom default constructor.
        /// @tparam T Type.
        template <typename T, typename F>
        static RemoteTrait createCustom(F defaultConstructor)
        {
            return RemoteTrait{ConstructibleTrait::typed<T>()
                                   .withCopyConstructor()
                                   .withMoveConstructor()
                                   .build()
                                   .withDefaultConstructor(memory::move(defaultConstructor))};
        }
    };
} // namespace cubos::core::reflection
