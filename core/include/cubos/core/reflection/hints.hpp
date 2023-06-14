#pragma once

#include <memory>

#include <cubos/core/reflection/type_map.hpp>

namespace cubos::core::reflection
{
    /// @brief Stores reflection hints of any type. The hints themselves must be reflectable.
    class Hints
    {
    public:
        /// @brief Adds a hint to the map.
        /// @param type Type of the hint, must match the given data.
        /// @param data Data of the hint.
        void add(const Type& type, std::shared_ptr<void>&& data);

        /// @brief Adds a hint to the map.
        /// @tparam T Type of the hint, must match the given data.
        /// @param data Data of the hint.
        template <typename T>
        void add(T&& data)
        {
            this->add(reflect<T>(), std::make_shared<T>(std::move(data)));
        }

        /// @brief Gets a pointer to the hint of the given type.
        /// @param type Type of the hint.
        /// @return Pointer to the hint, or `nullptr` if the hint does not exist.
        const void* get(const Type& type) const;

        /// @brief Gets a pointer to the hint of the given type.
        /// @tparam T Type of the hint.
        /// @return Pointer to the hint, or `nullptr` if the hint does not exist.
        template <typename T>
        const T* get() const
        {
            return static_cast<const T*>(this->get(reflect<T>()));
        }

        /// @brief Gets an iterator to the beginning of the hints map.
        /// @return Iterator to the hints map.
        auto begin() const
        {
            return mHints.begin();
        }

        /// @brief Gets an iterator to the end of the hints map.
        /// @return Iterator to the end of the hints map.
        auto end() const
        {
            return mHints.end();
        }

    private:
        TypeMap<std::shared_ptr<void>> mHints;
    };
} // namespace cubos::core::reflection
