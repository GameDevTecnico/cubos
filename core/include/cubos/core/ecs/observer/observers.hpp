/// @file
/// @brief Class @ref cubos::core::ecs::Observers.
/// @ingroup core-ecs-observer

#pragma once

#include <string>
#include <unordered_map>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/ecs/observer/id.hpp>
#include <cubos/core/ecs/system/system.hpp>
#include <cubos/core/ecs/table/column.hpp>

namespace cubos::core::ecs
{
    /// @brief Stores and manages all of the observers associated with a world.
    /// @ingroup core-ecs-observer
    class Observers
    {
    public:
        ~Observers();

        /// @brief Notifies that the given entity has a new column.
        /// @param commandBuffer Command buffer to record the any commands emitted by the observer.
        /// @param entity Entity.
        /// @param column Column.
        /// @return Whether an observer was triggered.
        bool notifyAdd(CommandBuffer& commandBuffer, Entity entity, ColumnId column);

        /// @brief Notifies that the given entity has lost a column.
        /// @param commandBuffer Command buffer to record the any commands emitted by the observer.
        /// @param entity Entity.
        /// @param column Column.
        /// @return Whether an observer was triggered.
        bool notifyRemove(CommandBuffer& commandBuffer, Entity entity, ColumnId column);

        /// @brief Hooks an observer to the addition of a column.
        /// @param column Column.
        /// @param observer Observer system.
        /// @return Observer identifier.
        ObserverId hookOnAdd(ColumnId column, System<void> observer);

        /// @brief Hooks an observer to the removal of a column.
        /// @param column Column.
        /// @param observer Observer system.
        /// @return Observer identifier.
        ObserverId hookOnRemove(ColumnId column, System<void> observer);

        /// @brief Unhooks an observer.
        /// @param id Observer identifier.
        void unhook(ObserverId id);

    private:
        std::vector<System<void>*> mObservers; /// Indexed by observer identifier.
        std::unordered_multimap<ColumnId, ObserverId, ColumnIdHash> mOnAdd;
        std::unordered_multimap<ColumnId, ObserverId, ColumnIdHash> mOnRemove;
    };
} // namespace cubos::core::ecs
