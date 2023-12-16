/// @file
/// @brief Class @ref cubos::core::ecs::Tables.
/// @ingroup core-ecs-table

#pragma once

#include <unordered_map>

#include <cubos/core/ecs/entity/archetype_graph.hpp>
#include <cubos/core/ecs/table/dense.hpp>

namespace cubos::core::ecs
{
    /// @brief Stores the tables of a given world.
    /// @ingroup core-ecs-table
    class Tables final
    {
    public:
        /// @brief Constructs.
        Tables();

        /// @brief Checks if the given archetype has a dense table.
        /// @param archetype Archetype identifier.
        /// @return WHether it contains a table or not.
        bool contains(ArchetypeId archetype) const;

        /// @brief Returns a reference to the dense table of the given archetype.
        ///
        /// Creates the table if it doesn't exist already.
        ///
        /// @param archetype Archetype identifier.
        /// @param graph Archetype graph used to initialize the table, if necessary.
        /// @param typew Type registry used to initialize the table, if necessary.
        /// @return Reference to table.
        DenseTable& dense(ArchetypeId archetype, ArchetypeGraph& graph, Types& types);

        /// @brief Returns a reference to the dense table of the given archetype.
        ///
        /// Aborts if the table doesn't exist.
        ///
        /// @param archetype Archetype identifier.
        /// @return Reference to table.
        DenseTable& dense(ArchetypeId archetype);

        /// @copydoc dense(ArchetypeId)
        const DenseTable& dense(ArchetypeId archetype) const;

    private:
        std::unordered_map<std::size_t, DenseTable> mDenseTables;
    };
} // namespace cubos::core::ecs
