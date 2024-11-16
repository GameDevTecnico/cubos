/// @file
/// @brief Struct @ref cubos::core::ecs::Entity.
/// @ingroup core-ecs-entity

#pragma once

#include <bitset>
#include <cstdint>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::ecs
{
    /// @brief Identifies an entity.
    ///
    /// When serializing/deserializing, if there's a
    /// data::old::SerializationMap<Entity, std::string, EntityHash> in the context, it will be used to
    /// (de)serialize strings representing the entities. Otherwise, the identifiers will be
    /// (de)serialized as objects with two fields: their index and their generation.
    ///
    /// @ingroup core-ecs-entity
    struct CUBOS_CORE_API Entity
    {
        CUBOS_REFLECT;

        /// @brief Constructs a null entity.
        Entity();

        /// @brief Constructs an entity from an index and a generation.
        /// @param index Entity index.
        /// @param generation Entity generation.
        Entity(uint32_t index, uint32_t generation);

        /// @brief Copy constructs.
        /// @param entity Entity to copy.
        Entity(const Entity& entity) = default;

        /// @brief Copy assigns.
        /// @param entity Entity to copy.
        Entity& operator=(const Entity& entity) = default;

        /// @brief Checks if the entity is equal to another.
        /// @param entity Entity to compare to.
        /// @return Whether the entities are equal.
        bool operator==(const Entity& entity) const = default;

        /// @brief Checks if the entity is not equal to another.
        /// @param entity Entity to compare to.
        /// @return Whether the entities are not equal.
        bool operator!=(const Entity& entity) const = default;

        /// @brief Checks if the entity is null, a special value returned on errors.
        /// @return Whether the entity is null.
        bool isNull() const;

        /// @brief Identifies the entity among the living entities in a world.
        ///
        /// While this is unique at a given time, it may be reused after the entity is destroyed.
        uint32_t index;

        /// @brief Counts how many entities with the same index have previously been destroyed.
        ///
        /// Allows us to detect when an entity has been destroyed by comparing its generation with
        /// the current generation of the entity with the same index.
        uint32_t generation;
    };
} // namespace cubos::core::ecs
