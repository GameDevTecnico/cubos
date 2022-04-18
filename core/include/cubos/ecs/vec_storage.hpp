#ifndef ECS_VEC_STORAGE_HPP
#define ECS_VEC_STORAGE_HPP

#include <cubos/ecs/storage.hpp>

namespace cubos::ecs
{

    template <typename T> class VecStorage : public Storage<T>
    {
    public:
        T* insert(uint32_t index, T value) override;
        T* get(uint32_t index) override;
        void erase(uint32_t index) override;

    private:
        std::vector<T> data;
    };

    template <typename T> T* VecStorage<T>::insert(uint32_t index, T value)
    {
        if (data.size() <= index)
        {
            data.resize(index);
            data.push_back(value);
        }
        else
        {
            data[index] = value;
        }

        return &data[index];
    }

    template <typename T> T* VecStorage<T>::get(uint32_t index)
    {
        return &data[index];
    }

    template <typename T> void VecStorage<T>::erase(uint32_t index)
    {
        data[index].~T();
        new (&data[index]) T;
    }

} // namespace cubos::ecs

#endif // ECS_VEC_STORAGE_HPP
