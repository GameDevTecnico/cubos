#ifndef ECS_NULL_STORAGE_HPP
#define ECS_NULL_STORAGE_HPP

#include <cubos/ecs/storage.hpp>

namespace cubos::ecs
{

    template <typename T> class NullStorage : public Storage<T>
    {
    public:
        T* insert(uint32_t index, T value) override;
        T* get(uint32_t index) override;
        void erase(uint32_t index) override;

    private:
        T data;
    };

    template <typename T> T* NullStorage<T>::insert(uint32_t index, T value)
    {
        return &data;
    }

    template <typename T> T* NullStorage<T>::get(uint32_t index)
    {
        return &data;
    }

    template <typename T> void NullStorage<T>::erase(uint32_t index)
    {
    }

} // namespace cubos::ecs

#endif // ECS_NULL_STORAGE_HPP
