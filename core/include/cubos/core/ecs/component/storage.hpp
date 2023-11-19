/// @file
/// @brief Class @ref cubos::core::ecs::Storage and related types.
/// @ingroup core-ecs-component

#pragma once

#include <cubos/core/ecs/entity/manager.hpp>

namespace cubos::core::ecs
{
    /// @brief Abstract parent class for all storages.
    ///
    /// Necessary to provide a type-erased interface for erasing and packaging/unpackaging
    /// components.
    ///
    /// @ingroup core-ecs-component
    class IStorage
    {
    public:
        virtual ~IStorage() = default;

        /// @brief Inserts a value into the storage.
        /// @param index Index where to insert the value.
        /// @param value Value to be moved.
        virtual void insert(uint32_t index, void* value) = 0;

        /// @brief Remove a value from the storage.
        /// @param index Index of the value to be removed.
        virtual void erase(uint32_t index) = 0;

        /// @brief Gets a value from the storage.
        /// @param index Index of the value to be retrieved.
        /// @return Pointer to the value.
        virtual void* get(uint32_t index) = 0;

        /// @brief Gets a value from the storage.
        /// @param index Index of the value to be retrieved.
        /// @return Pointer to the value.
        virtual const void* get(uint32_t index) const = 0;
    };

    /// @brief Abstract container for a component type @p T.
    /// @tparam T Component type.
    /// @ingroup core-ecs-component
    template <typename T>
    class Storage : public IStorage
    {
    public:
        /// @brief Component type.
        using Type = T;
    };
} // namespace cubos::core::ecs
