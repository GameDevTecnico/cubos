/// @file
/// @brief Struct @ref cubos::core::ecs::DenseTableId.
/// @ingroup core-ecs-table

#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include <cubos/core/ecs/types.hpp>

namespace cubos::core::ecs
{
    /// @brief Identifies a data column type.
    /// @ingroup core-ecs-table
    struct ColumnId
    {
        uint64_t inner; ///< Column type identifier.

        static const ColumnId Invalid; ///< Invalid column type identifier.

        /// @brief Creates a column type identifier from a data type identifier.
        /// @param id Data type identifier.
        /// @return Column type identifier.
        static ColumnId make(DataTypeId id);

        /// @brief Creates a column type identifier from a data type identifier and an index.
        /// @param id Data type identifier.
        /// @param index Index.
        /// @return Column type identifier.
        static ColumnId make(DataTypeId id, uint32_t index);

        /// @brief Returns the data type identifier of this column type.
        /// @return Data type identifier.
        DataTypeId dataType() const;

        /// @brief Returns the index of this column type.
        /// @return Index.
        uint32_t index() const;

        /// @brief Compares two column type identifiers for equality.
        /// @param other Other column type identifier.
        /// @return Whether the two column type identifiers are equal.
        bool operator==(const ColumnId& other) const = default;
    };

    /// @brief Used to hash @ref ColumnId objects.
    /// @ingroup core-ecs-table
    struct ColumnIdHash
    {
        std::size_t operator()(const ColumnId& columnId) const noexcept;
    };
} // namespace cubos::core::ecs
