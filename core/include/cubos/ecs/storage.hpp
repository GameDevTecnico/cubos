#ifndef ECS_STORAGE_HPP
#define ECS_STORAGE_HPP

#include <unordered_map>
#include <vector>

namespace cubos::ecs
{
    class IStorage
    {
    };

    template <typename T> class Storage : public IStorage
    {
    public:
        virtual T* insert(size_t index, T value) = 0;
        virtual T* get(size_t index) = 0;
    };

    template <typename T> class VecStorage : public Storage<T>
    {
    private:
        std::vector<T> _data;

    public:
        T* insert(size_t index, T value) override;

        T* get(size_t index) override;
    };

    template <typename T> class MapStorage : public Storage<T>
    {
    private:
        std::unordered_map<size_t, T> _data;

    public:
        T* insert(size_t index, T value) override;

        T* get(size_t index) override;
    };

    template <typename T> class NullStorage : public Storage<T>
    {
    private:
        T _data;

    public:
        T* insert(size_t index, T value) override;

        T* get(size_t index) override;
    };

    template <typename T> T* VecStorage<T>::insert(size_t index, T value)
    {
        if (_data.size() <= index)
        {
            _data.resize(index);
            _data.push_back(value);
        }
        else
        {
            _data[index] = value;
        }

        return &_data[index];
    }

    template <typename T> T* VecStorage<T>::get(size_t index)
    {
        return &_data[index];
    }

    template <typename T> T* MapStorage<T>::insert(size_t index, T value)
    {
        _data[index] = value;
        return &_data[index];
    }

    template <typename T> T* MapStorage<T>::get(size_t index)
    {
        return &_data[index];
    }

    template <typename T> T* NullStorage<T>::insert(size_t index, T value)
    {
        return &_data;
    }

    template <typename T> T* NullStorage<T>::get(size_t index)
    {
        return &_data;
    }
} // namespace cubos::ecs

#endif // ECS_STORAGE_HPP
