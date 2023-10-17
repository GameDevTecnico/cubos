/// @file
/// @brief Class @ref cubos::core::ecs::NullStorage.
/// @ingroup core-ecs-component

#pragma once

#include <cubos/core/ecs/component/storage.hpp>

namespace cubos::core::ecs
{

    /// @brief Storage implementation that doesn't keep any data, made for
    /// zero-sized components.
    /// @tparam T Component type.
    /// @ingroup core-ecs-component
    template <typename T>
    class NullStorage : public Storage<T>
    {
    public:
        void insert(uint32_t index, void* value) override;
        void* get(uint32_t index) override;
        const void* get(uint32_t index) const override;
        void erase(uint32_t index) override;

    private:
        T mData;
    };

    template <typename T>
    void NullStorage<T>::insert(uint32_t /*unused*/, void* /*unused*/)
    {
    }

    template <typename T>
    void* NullStorage<T>::get(uint32_t /*unused*/)
    {
        return &mData;
    }

    template <typename T>
    const void* NullStorage<T>::get(uint32_t /*unused*/) const
    {
        return &mData;
    }

    template <typename T>
    void NullStorage<T>::erase(uint32_t /*unused*/)
    {
    }

} // namespace cubos::core::ecs
