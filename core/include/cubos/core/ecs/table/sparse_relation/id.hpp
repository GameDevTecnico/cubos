/// @file
/// @brief Struct @ref cubos::core::ecs::SparseRelationTableId.
/// @ingroup core-ecs-table

#pragma once

#include <cubos/core/ecs/entity/archetype_id.hpp>
#include <cubos/core/ecs/types.hpp>

namespace cubos::core::reflection
{
    class Type;
} // namespace cubos::core::reflection

namespace cubos::core::ecs
{
    /// @brief Identifies a sparse relation table.
    /// @ingroup core-ecs-table
    struct SparseRelationTableId
    {
        DataTypeId dataType{DataTypeId::Invalid};
        ArchetypeId from{ArchetypeId::Invalid};
        ArchetypeId to{ArchetypeId::Invalid};
        int depth{0};

        /// @brief Default constructor.
        SparseRelationTableId() = default;

        /// @brief Constructs.
        /// @param dataType Relation data type.
        /// @param from From archetype identifier.
        /// @param to To archetype identifier.
        /// @param depth Depth of the relation. Used in tree relations, always 0 for others.
        SparseRelationTableId(DataTypeId dataType, ArchetypeId from, ArchetypeId to, int depth = 0);

        /// @brief Compares with another identifier.
        /// @param other Identifier.
        /// @return Whether they're equal.
        bool operator==(const SparseRelationTableId& other) const = default;
    };

    /// @brief Hash functor for @ref SparseRelationTableId.
    struct SparseRelationTableIdHash
    {
        std::size_t operator()(const SparseRelationTableId& id) const;
    };
} // namespace cubos::core::ecs
