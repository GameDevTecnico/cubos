#ifndef CUBOS_CORE_ECS_STORAGE_HPP
#define CUBOS_CORE_ECS_STORAGE_HPP

#include <cubos/core/data/handle.hpp>
#include <cubos/core/data/package.hpp>
#include <cubos/core/data/serialization_map.hpp>

#include <cubos/core/ecs/entity_manager.hpp>

#include <unordered_map>
#include <vector>
#include <cstdint>
#include <typeindex>

namespace cubos::core::ecs
{
    /// IStorage is an abstract parent class for all storages.
    class IStorage
    {
    public:
        virtual ~IStorage() = default;

        /// @brief Remove a value from the storage.
        /// @param index The index of the value to be removed.
        virtual void erase(uint32_t index) = 0;

        /// @brief Packages a value. If the value doesn't exist, undefined behavior will occur.
        /// @param index The index of the value to package.
        /// @param map Serialization map needed for serialization.
        /// @returns The packaged value.
        virtual data::Package pack(uint32_t index, const data::SerializationMap<Entity, std::string>& map,
                                   data::Handle::SerContext handleCtx) const = 0;

        /// @brief Unpackages a value.
        /// @param index The index of the value to unpackage.
        /// @param package The package to unpackage.
        /// @param map Serialization map needed for deserialization.
        /// @returns True if the value was unpackaged successfully, false otherwise.
        virtual bool unpack(uint32_t index, const data::Package& package,
                            const data::SerializationMap<Entity, std::string>& map,
                            data::Handle::DesContext handleCtx) = 0;

        /// @brief Gets the type the components being stored here.
        virtual std::type_index type() const = 0;
    };

    /// @brief Storage is an abstract container for a certain type with common operations, such as,
    /// insert, get and erase.
    /// @tparam T The type to be stored in the storage.
    template <typename T> class Storage : public IStorage
    {
    public:
        using Type = T;

        /// @brief Inserts a value into the storage.
        /// @param index The index where to insert the value.
        /// @param value The value to be inserted.
        virtual T* insert(uint32_t index, T value) = 0;

        /// @brief Gets a value from the storage.
        /// @param index The index of the value to be retrieved.
        virtual T* get(uint32_t index) = 0;

        /// @brief Gets a value from the storage.
        /// @param index The index of the value to be retrieved.
        virtual const T* get(uint32_t index) const = 0;

        // Implementation.

        virtual data::Package pack(uint32_t index, const data::SerializationMap<Entity, std::string>& map,
                                   data::Handle::SerContext handleCtx) const override;
        virtual bool unpack(uint32_t index, const data::Package& package,
                            const data::SerializationMap<Entity, std::string>& map,
                            data::Handle::DesContext handleCtx) override;
        virtual std::type_index type() const override;
    };

    // Implementation.

    template <typename T>
    data::Package Storage<T>::pack(uint32_t index, const data::SerializationMap<Entity, std::string>& map,
                                   data::Handle::SerContext handleCtx) const
    {
        return data::Package::from(*this->get(index), std::forward_as_tuple(map, handleCtx));
    }

    template <typename T>
    bool Storage<T>::unpack(uint32_t index, const data::Package& package,
                            const data::SerializationMap<Entity, std::string>& map, data::Handle::DesContext handleCtx)
    {
        T value;
        if (package.into(value, std::forward_as_tuple(map, handleCtx)))
        {
            this->insert(index, value);
            return true;
        }

        return false;
    }

    template <typename T> std::type_index Storage<T>::type() const
    {
        return std::type_index(typeid(T));
    }
} // namespace cubos::core::ecs

#endif // ECS_STORAGE_HPP
