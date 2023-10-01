/// @file
/// @brief Class @ref cubos::core::ecs::Storage and related types.
/// @ingroup core-ecs-component

#pragma once

#include <cubos/core/data/old/package.hpp>
#include <cubos/core/data/old/serialization_map.hpp>
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

        /// @brief Remove a value from the storage.
        /// @param index Index of the value to be removed.
        virtual void erase(uint32_t index) = 0;

        /// @brief Packages a value. If the value doesn't exist, undefined behavior will occur.
        /// @param index Index of the value to package.
        /// @param context Optional context used for serialization.
        /// @return Packaged value.
        virtual data::old::Package pack(uint32_t index, data::old::Context* context) const = 0;

        /// @brief Unpackages a value.
        /// @param index Index of the value to unpackage.
        /// @param package Package to unpackage.
        /// @param context Optional context used for deserialization.
        /// @return Whether the unpackaging was successful.
        virtual bool unpack(uint32_t index, const data::old::Package& package, data::old::Context* context) = 0;

        /// @brief Gets the type the components being stored here.
        /// @return Component type.
        virtual std::type_index type() const = 0;
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

        /// @brief Inserts a value into the storage.
        /// @param index Index where to insert the value.
        /// @param value Value to be inserted.
        virtual T* insert(uint32_t index, T value) = 0;

        /// @brief Gets a value from the storage.
        /// @param index Index of the value to be retrieved.
        /// @return Pointer to the value.
        virtual T* get(uint32_t index) = 0;

        /// @brief Gets a value from the storage.
        /// @param index Index of the value to be retrieved.
        /// @return Pointer to the value.
        virtual const T* get(uint32_t index) const = 0;

        // Implementation.

        inline data::old::Package pack(uint32_t index, data::old::Context* context) const override
        {
            return data::old::Package::from(*this->get(index), context);
        }

        inline bool unpack(uint32_t index, const data::old::Package& package, data::old::Context* context) override
        {
            T value;
            if (package.into(value, context))
            {
                this->insert(index, std::move(value));
                return true;
            }

            return false;
        }

        inline std::type_index type() const override
        {
            return std::type_index(typeid(T));
        }
    };
} // namespace cubos::core::ecs
