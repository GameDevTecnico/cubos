#ifndef ECS_MAP_STORAGE_HPP
#define ECS_MAP_STORAGE_HPP

#include <cubos/ecs/storage.hpp>

namespace cubos::ecs
{

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

} // namespace cubos::ecs

#endif // ECS_MAP_STORAGE_HPP
