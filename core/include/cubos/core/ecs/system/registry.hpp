/// @file
/// @brief Class @ref cubos::core::ecs::SystemRegistry.
/// @ingroup core-ecs-system

#pragma once

#include <string>
#include <vector>

#include <cubos/core/ecs/system/system.hpp>

namespace cubos::core::ecs
{
    /// @brief Identifies a system.
    /// @ingroup core-ecs-system
    struct SystemId
    {
        std::size_t inner; ///< System identifier.

        /// @brief Compares two identifiers for equality.
        /// @param other Other identifier.
        /// @return Whether the two identifiers are equal.
        bool operator==(const SystemId& other) const = default;
    };

    /// @brief Identifies a condition.
    /// @ingroup core-ecs-system
    struct ConditionId
    {
        std::size_t inner; ///< Condition identifier.

        /// @brief Compares two identifiers for equality.
        /// @param other Other identifier.
        /// @return Whether the two identifiers are equal.
        bool operator==(const ConditionId& other) const = default;
    };

    /// @brief Stores known systems and conditions.
    /// @ingroup core-ecs-system
    class CUBOS_CORE_API SystemRegistry
    {
    public:
        ~SystemRegistry();

        /// @brief Constructs.
        SystemRegistry() = default;

        /// @brief Forbid copying.
        SystemRegistry(const SystemRegistry&) = delete;

        /// @brief Forbid copy assignment.
        SystemRegistry& operator=(const SystemRegistry&) = delete;

        /// @brief Removes all systems and conditions.
        ///
        /// Previously returned identifiers become invalid.
        /// This operation is equivalent to constructing a new registry.
        void reset();

        /// @brief Registers the given system.
        /// @param name Debug name.
        /// @param system System.
        /// @return System identifier.
        SystemId add(std::string name, System<void> system);

        /// @brief Registers the given condition.
        /// @param name Debug name.
        /// @param system System.
        /// @return Condition identifier.
        ConditionId add(std::string name, System<bool> condition);

        /// @brief Unregisters the given system.
        /// @param id System identifier.
        void remove(SystemId id);

        /// @brief Unregisters the given condition.
        /// @param id Condition identifier.
        void remove(ConditionId id);

        /// @brief Gets the debug name of the given system.
        /// @param id System identifier.
        /// @return System name.
        const std::string& name(SystemId id) const;

        /// @brief Gets the debug name of the given condition.
        /// @param id Condition identifier.
        /// @return Condition name.
        const std::string& name(ConditionId id) const;

        /// @brief Gets the system with the given identifier.
        /// @param id System identifier.
        System<void>& system(SystemId id);

        /// @brief Gets the condition with the given identifier.
        /// @param id Condition identifier.
        System<bool>& condition(ConditionId id);

    private:
        std::vector<std::string> mSystemNames;
        std::vector<std::string> mConditionNames;

        std::vector<System<void>*> mSystems;
        std::vector<System<bool>*> mConditions;
    };
} // namespace cubos::core::ecs
