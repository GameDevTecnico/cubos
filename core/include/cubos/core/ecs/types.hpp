/// @file
/// @brief Class @ref cubos::core::ecs::Types.
/// @ingroup core-ecs

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <cubos/core/memory/type_map.hpp>
#include <cubos/core/reflection/type_registry.hpp>

namespace cubos::core::ecs
{
    /// @brief Identifies a data type registered in the world.
    struct DataTypeId
    {
        uint32_t inner; ///< Data type identifier.

        static const DataTypeId Invalid; ///< Invalid data type identifier.

        /// @brief Compares two data type identifiers for equality.
        /// @param other Other data type identifier.
        /// @return Whether the two data type identifiers are equal.
        bool operator==(const DataTypeId& other) const = default;
    };

    /// @brief Hash functor for @ref DataTypeId.
    struct DataTypeIdHash
    {
        std::size_t operator()(const DataTypeId& id) const;
    };

    /// @brief Registry of all data types used in an ECS world.
    class Types final
    {
    public:
        /// @brief Registers a component type.
        /// @param type Component type.
        void addComponent(const reflection::Type& type);

        /// @brief Registers a relation type.
        /// @param type Relation type.
        void addRelation(const reflection::Type& type);

        /// @brief Gets the identifier of a data type.
        ///
        /// Aborts if the data type is not registered.
        ///
        /// @param type Data type.
        /// @return Data type identifier.
        DataTypeId id(const reflection::Type& type) const;

        /// @brief Gets the identifier of a data type.
        ///
        /// Aborts if the data type is not registered.
        ///
        /// @param name Data type name.
        /// @return Data type identifier.
        DataTypeId id(const std::string& name) const;

        /// @brief Gets a data type from its identifier.
        /// @param id Data type identifier.
        /// @return Data type.
        const reflection::Type& type(DataTypeId id) const;

        /// @brief Checks if a data type is registered.
        /// @param name Data type.
        /// @return Whether the data type is registered.
        bool contains(const reflection::Type& type) const;

        /// @brief Checks if a data type is registered.
        /// @param name Data type name.
        /// @return Whether the data type is registered.
        bool contains(const std::string& name) const;

        /// @brief Checks if the given data type is a component.
        /// @param id Data type identifier.
        /// @return Whether the identifier refers to a component.
        bool isComponent(DataTypeId id) const;

        /// @brief Checks if the given data type is a relation.
        /// @param id Data type identifier.
        /// @return Whether the identifier refers to a relation.
        bool isRelation(DataTypeId id) const;

        /// @brief Checks if the given data type is a symmetric relation.
        /// @param id Data type identifier.
        /// @return Whether the identifier refers to a symmetric relation.
        bool isSymmetricRelation(DataTypeId id) const;

        /// @brief Checks if the given data type is a tree relation.
        /// @param id Data type identifier.
        /// @return Whether the identifier refers to a tree relation.
        bool isTreeRelation(DataTypeId id) const;

        /// @brief Gets a type registry with only the component types.
        /// @return Component type registry.
        reflection::TypeRegistry components() const;

        /// @brief Gets a type registry with only the relation types.
        /// @return Relation type registry.
        reflection::TypeRegistry relations() const;

    private:
        /// @brief Possible data type kinds.
        enum class Kind
        {
            Component,
            Relation
        };

        /// @brief Describes a data type.
        struct Entry
        {
            const reflection::Type* type; ///< Data type.
            Kind kind;                    ///< Data type kind.
            bool isSymmetric;             ///< Whether the relation is symmetric (ignored for components).
            bool isTree;                  ///< Whether the relation forms trees (ignored for components).
        };

        /// @brief Registers a new data type.
        /// @param type Data type.
        /// @param kind Data type kind.
        void add(const reflection::Type& type, Kind kind);

        memory::TypeMap<DataTypeId> mTypes;                 ///< Maps data types to their identifiers.
        std::unordered_map<std::string, DataTypeId> mNames; ///< Maps data type names to their identifiers.
        std::vector<Entry> mEntries;                        ///< Stores data type information.
    };
} // namespace cubos::core::ecs
