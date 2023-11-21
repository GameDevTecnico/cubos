/// @file
/// @brief Class @ref cubos::core::ecs::Storage and related types.
/// @ingroup core-ecs-component

#pragma once

#include <cstdint>

#include <cubos/core/memory/any_vector.hpp>

namespace cubos::core::ecs
{
    /// @brief Stores the components of a given type.
    /// @ingroup core-ecs-component
    class Storage final
    {
    public:
        /// @brief Constructs.
        /// @param type Component type.
        Storage(const reflection::Type& type);

        /// @brief Move constructs.
        /// @param other Moved storage.
        Storage(Storage&& other) noexcept = default;

        /// @brief Inserts a value into the storage.
        /// @param index Index where to insert the value.
        /// @param value Value to be moved.
        void insert(uint32_t index, void* value);

        /// @brief Remove a value from the storage.
        /// @param index Index of the value to be removed.
        void erase(uint32_t index);

        /// @brief Gets a value from the storage.
        /// @param index Index of the value to be retrieved.
        /// @return Pointer to the value.
        void* get(uint32_t index);

        /// @brief Gets a value from the storage.
        /// @param index Index of the value to be retrieved.
        /// @return Pointer to the value.
        const void* get(uint32_t index) const;

        /// @brief Gets the component type stored in the storage.
        /// @return Component type.
        const reflection::Type& type() const;

    private:
        memory::AnyVector mData;
        const reflection::ConstructibleTrait* mConstructibleTrait{nullptr};
    };
} // namespace cubos::core::ecs
