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

    /// @brief Registry of all data types used in an ECS world.
    class Types final
    {
    public:
        /// @brief Registers a component type.
        /// @param type Component type.
        void addComponent(const reflection::Type& type);

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
        /// @param name Data type name.
        /// @return Whether the data type is registered.
        bool contains(const std::string& name) const;

        /// @brief Checks which kind of data type a given identifier refers to.
        /// @param id Data type identifier.
        /// @return Whether the identifier refers to a component.
        bool isComponent(DataTypeId id) const;

        /// @brief Gets a type registry with only the component types.
        /// @return Component type registry.
        reflection::TypeRegistry components() const;

    private:
        /// @brief Describes a data type.
        struct Entry
        {
            const reflection::Type* type; ///< Data type.
            bool isComponent;             ///< Whether the data type is a component.
        };

        memory::TypeMap<DataTypeId> mTypes;                 ///< Maps data types to their identifiers.
        std::unordered_map<std::string, DataTypeId> mNames; ///< Maps data type names to their identifiers.
        std::vector<Entry> mEntries;                        ///< Stores data type information.
    };
} // namespace cubos::core::ecs
