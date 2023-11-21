/// @file
/// @brief Class @ref cubos::core::ecs::ComponentManager and related types.
/// @ingroup core-ecs-component

#pragma once

#include <cstdint>
#include <vector>

#include <cubos/core/ecs/component/storage.hpp>
#include <cubos/core/memory/type_map.hpp>

namespace cubos::core::ecs
{
    /// @brief Holds and manages components.
    ///
    /// Used internally by @ref World.
    ///
    /// @ingroup core-ecs-component
    class ComponentManager final
    {
    public:
        /// @brief Registers a new component type with the component manager.
        /// @param type Type of the component.
        void registerType(const reflection::Type& type);

        /// @brief Gets the identifier of a registered component type.
        /// @param type Component type.
        /// @return Component identifier.
        uint32_t id(const reflection::Type& type) const;

        /// @brief Gets the identifier of a registered component type.
        /// @tparam T Component type.
        /// @return Component identifier.
        template <reflection::Reflectable T>
        uint32_t id() const
        {
            return this->id(reflection::reflect<T>());
        }

        /// @brief Gets the type of a component from its identifier.
        /// @param id Component identifier.
        /// @return Component type index.
        const reflection::Type& type(uint32_t id) const;

        /// @brief Adds a component to an entity.
        /// @param index Entity index.
        /// @param id Component identifier.
        /// @param value Component value to move.
        void insert(uint32_t index, uint32_t id, void* value);

        /// @brief Removes a component from an entity.
        /// @param index Entity index.
        /// @param id Component identifier.
        void erase(uint32_t index, uint32_t id);

        /// @brief Removes all components from an entity.
        /// @param index Entity index.
        void erase(uint32_t index);

        /// @brief Gets the storage of the given component type.
        /// @param id Component identifier.
        /// @return Component storage.
        Storage& storage(uint32_t id);

        /// @copydoc storage(uint32_t)
        const Storage& storage(uint32_t id) const;

    private:
        memory::TypeMap<uint32_t> mTypeToIds; ///< Maps component types to component IDs.
        std::vector<Storage> mStorages;       ///< Registered component storages.
    };
} // namespace cubos::core::ecs
