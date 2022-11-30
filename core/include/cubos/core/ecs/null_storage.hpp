#ifndef CUBOS_CORE_ECS_NULL_STORAGE_HPP
#define CUBOS_CORE_ECS_NULL_STORAGE_HPP

#include <cubos/core/ecs/storage.hpp>

namespace cubos::core::ecs
{

    /// @brief NullStorage is a Storage implementation that doesn't keep any data, made for components
    /// that don't hold any data and just work as tags.
    /// @tparam T The type to be stored in the storage.
    template <typename T> class NullStorage : public Storage<T>
    {
    public:
        T* insert(uint32_t index, T value) override;
        T* get(uint32_t index) override;
        const T* get(uint32_t index) const override;
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

    template <typename T> const T* NullStorage<T>::get(uint32_t index) const
    {
        return &data;
    }

    template <typename T> void NullStorage<T>::erase(uint32_t index)
    {
    }

} // namespace cubos::core::ecs

#endif // ECS_NULL_STORAGE_HPP
