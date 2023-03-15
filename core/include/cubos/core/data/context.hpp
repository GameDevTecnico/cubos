#ifndef CUBOS_CORE_DATA_CONTEXT_HPP
#define CUBOS_CORE_DATA_CONTEXT_HPP

#include <typeindex>
#include <vector>

namespace cubos::core::data
{
    /// Stores the context necessary to serialize or deserialize data.
    /// This is done through a type map, which stores an instance for each type added (akin to the
    /// ECS resources).
    class Context final
    {
    public:
        Context() = default;
        ~Context();

        /// Pushes a new value to the stack.
        /// @tparam T The type of the data.
        /// @param data The data to push.
        template <typename T>
        inline void push(T&& data)
        {
            auto copy = new T(std::forward<T>(data));
            auto destructor = [](void* data) { delete static_cast<T*>(data); };
            this->pushAny(typeid(T), copy, destructor);
        }

        /// Pops the top value from the stack.
        /// This must be called once for each push.
        void pop();

        /// Gets the data associated with the given type.
        /// If the type is present multiple times, the topmost one is returned.
        /// Aborts if the data is not present.
        /// @tparam T The type of the data.
        /// @returns the data associated with the given type.
        template <typename T>
        inline T& get()
        {
            return *static_cast<T*>(this->getAny(typeid(T)));
        }

    private:
        /// Data associated with a stored value.
        struct Entry
        {
            std::type_index type;      ///< Type of the data.
            void* data;                ///< Pointer to the data.
            void (*destructor)(void*); ///< Destructor for the data.
        };

        /// @param type The type of the data.
        /// @param data The data to push.
        /// @param destructor Destructor for the data.
        void pushAny(std::type_index type, void* data, void (*destructor)(void*));

        /// Aborts if the data is not present.
        /// @param type The type of the data.
        /// @returns a pointer to the data associated with the given type.
        void* getAny(std::type_index type);

        std::vector<Entry> entries; ///< Entries in the context.
    };
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_CONTEXT_HPP
