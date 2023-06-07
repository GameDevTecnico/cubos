#pragma once

#include <cubos/core/ecs/storage.hpp>

namespace cubos::core::ecs
{

    /// @brief VecStorage is a Storage implementation that uses a STL vector, this is best
    /// for components that are used very often, in order to be the most efficient memory wise.
    /// @tparam T The type to be stored in the storage.
    template <typename T>
    class VecStorage : public Storage<T>
    {
    public:
        T* insert(uint32_t index, T value) override;
        T* get(uint32_t index) override;
        const T* get(uint32_t index) const override;
        void erase(uint32_t index) override;

    private:
        std::vector<T> mData;
    };

    template <typename T>
    T* VecStorage<T>::insert(uint32_t index, T value)
    {
        if (mData.size() <= index)
        {
            mData.resize(index);
            mData.emplace_back(std::move(value));
        }
        else
        {
            mData[index].~T();
            new (&mData[index]) T(std::move(value));
        }

        return &mData[index];
    }

    template <typename T>
    T* VecStorage<T>::get(uint32_t index)
    {
        return &mData[index];
    }

    template <typename T>
    const T* VecStorage<T>::get(uint32_t index) const
    {
        return &mData[index];
    }

    template <typename T>
    void VecStorage<T>::erase(uint32_t index)
    {
        if (static_cast<size_t>(index) < mData.size())
        {
            mData[index].~T();
            new (&mData[index]) T;
        }
    }
} // namespace cubos::core::ecs
