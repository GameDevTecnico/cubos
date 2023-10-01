/// @file
/// @brief Class @ref cubos::core::ecs::MapStorage.
/// @ingroup core-ecs-component

#pragma once

#include <cubos/core/ecs/component/storage.hpp>

namespace cubos::core::ecs
{

    /// @brief Storage implementation that uses an `std::unordered_map`.
    /// @tparam T Component type.
    /// @ingroup core-ecs-component
    template <typename T>
    class MapStorage : public Storage<T>
    {
    public:
        T* insert(uint32_t index, T value) override;
        T* get(uint32_t index) override;
        const T* get(uint32_t index) const override;
        void erase(uint32_t index) override;

    private:
        std::unordered_map<uint32_t, T> mData;
    };

    template <typename T>
    T* MapStorage<T>::insert(uint32_t index, T value)
    {
        mData[index] = value;
        return &mData[index];
    }

    template <typename T>
    T* MapStorage<T>::get(uint32_t index)
    {
        return &mData.at(index);
    }

    template <typename T>
    const T* MapStorage<T>::get(uint32_t index) const
    {
        return &mData.at(index);
    }

    template <typename T>
    void MapStorage<T>::erase(uint32_t index)
    {
        mData.erase(index);
    }

} // namespace cubos::core::ecs
