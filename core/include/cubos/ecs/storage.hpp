#ifndef ECS_STORAGE_HPP
#define ECS_STORAGE_HPP

#include <unordered_map>
#include <vector>
#include <cstdint>

namespace cubos::ecs
{
    class IStorage
    {
    public:
        virtual ~IStorage() = default;
    };

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

        virtual void erase(uint32_t index) = 0;
    };

} // namespace cubos::ecs

#endif // ECS_STORAGE_HPP
