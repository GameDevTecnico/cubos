#ifndef ECS_STORAGE_HPP
#define ECS_STORAGE_HPP

#include <unordered_map>
#include <vector>
#include <cstdint>

namespace cubos::ecs
{
    class IStorage
    {
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

    template <typename T> class VecStorage : public Storage<T>
    {
    private:
        std::vector<T> data;

    public:
        T* insert(uint32_t index, T value) override;

        T* get(uint32_t index) override;

        void erase(uint32_t index) override;
    };

    template <typename T> class MapStorage : public Storage<T>
    {
    private:
        std::unordered_map<uint32_t, T> data;

    public:
        T* insert(uint32_t index, T value) override;

        T* get(uint32_t index) override;

        void erase(uint32_t index) override;
    };

    template <typename T> class NullStorage : public Storage<T>
    {
    private:
        T data;

    public:
        T* insert(uint32_t index, T value) override;

        T* get(uint32_t index) override;

        void erase(uint32_t index) override;
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

#endif // ECS_STORAGE_HPP
