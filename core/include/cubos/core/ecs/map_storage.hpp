#ifndef CUBOS_CORE_ECS_MAP_STORAGE_HPP
#define CUBOS_CORE_ECS_MAP_STORAGE_HPP

#include <cubos/core/ecs/storage.hpp>

namespace cubos::core::ecs
{

    /// @brief MapStorage is a Storage implementation that uses a STL unordered_map, this is best
    /// for components that aren't used that often so the data is stored in a more compact way.
    /// @tparam T The type to be stored in the storage.
    template <typename T> class MapStorage : public Storage<T>
    {
    public:
        T* insert(uint32_t index, T value) override;
        T* get(uint32_t index) override;
        void erase(uint32_t index) override;

    private:
        std::unordered_map<uint32_t, T> data;
    };

    template <typename T> T* MapStorage<T>::insert(uint32_t index, T value)
    {
        data[index] = value;
        return &data[index];
    }

    template <typename T> T* MapStorage<T>::get(uint32_t index)
    {
        return &data[index];
    }

    template <typename T> void MapStorage<T>::erase(uint32_t index)
    {
        data.erase(index);
    }

} // namespace cubos::core::ecs

#endif // ECS_MAP_STORAGE_HPP
