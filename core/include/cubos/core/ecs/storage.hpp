#ifndef CUBOS_CORE_ECS_STORAGE_HPP
#define CUBOS_CORE_ECS_STORAGE_HPP

#include <unordered_map>
#include <vector>
#include <cstdint>

namespace cubos::core::ecs
{

    /// IStorage is an abstract parent class for all storages.
    class IStorage
    {
    public:
        virtual ~IStorage() = default;
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

        /// @brief Remove a value from the storage.
        /// @param index The index of the value to be removed.
        virtual void erase(uint32_t index) = 0;
    };

} // namespace cubos::core::ecs

#endif // ECS_STORAGE_HPP
