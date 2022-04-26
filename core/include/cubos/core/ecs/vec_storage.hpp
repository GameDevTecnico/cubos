#ifndef CUBOS_CORE_ECS_VEC_STORAGE_HPP
#define CUBOS_CORE_ECS_VEC_STORAGE_HPP

#include <cubos/core/ecs/storage.hpp>

namespace cubos::core::ecs
{

    /// @brief VecStorage is a Storage implementation that uses a STL vector, this is best
    /// for components that are used very often, in order to be the most efficient memory wise.
    /// @tparam T The type to be stored in the storage.
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

} // namespace cubos::core::ecs

#endif // ECS_VEC_STORAGE_HPP
