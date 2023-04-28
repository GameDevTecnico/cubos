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
        inline void push(T data)
        {
            auto copy = new T(std::forward<T>(data));
            auto destructor = [](void* data) { delete static_cast<T*>(data); };
            this->pushAny(typeid(T), copy, destructor);
        }

        /// Pushes a new sub-context to the stack.
        /// The sub-context acts as a group of values associated with a single push.
        /// The reference must be valid until the sub-context is popped.
        /// @param context The context to push.
        void pushSubContext(Context& context);

        /// Pops the top value or sub-context from the stack.
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

        /// Checks if there is data associated with the given type.
        /// @tparam T The type of the data.
        /// @returns True if the data is present, false otherwise.
        template <typename T>
        inline bool has() const
        {
            return this->tryGetAny(typeid(T)) != nullptr;
        }

    private:
        /// Data associated with either a stored value, or a sub-context.
        struct Entry
        {
            Context* subContext;       ///< If nullptr, this is a value, otherwise it's a sub-context.
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
        /// @returns A pointer to the data associated with the given type.
        void* getAny(std::type_index type) const;

        /// Tries to get the data associated with the given type.
        /// @param type The type of the data.
        /// @returns A pointer to the data associated with the given type, or nullptr if it's not
        /// present.
        void* tryGetAny(std::type_index type) const;

        std::vector<Entry> mEntries; ///< Entries in the context.
    };
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_CONTEXT_HPP
