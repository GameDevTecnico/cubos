#ifndef CUBOS_MEMORY_FACTORY_HPP
#define CUBOS_MEMORY_FACTORY_HPP

#include <cubos/log.hpp>

#include <string>
#include <functional>

namespace cubos::memory
{
    /// Generic class used to create new instances derived from a given type, from their identifier.
    /// Objects are allocated using new, and must be deleted using delete.
    ///
    /// @tparam T The base type to be created.
    /// @tparam TArgs The types of the arguments to be passed to the constructor.
    template <typename T, typename... TArgs> class Factory
    {
    public:
        Factory() = delete;
        ~Factory() = delete;

        /// Registers a new type to be created on the factory.
        /// @param id The identifier of the type to be created.
        /// @param constructor The constructor to be used to create the type.
        static void registerType(const std::string& id, std::function<T*(TArgs...)> constructor);

        /// Creates a new instance of the type with the given identifier.
        /// If the type is not registered, nullptr is called.
        /// Must be destroyed using delete.
        /// @param id The identifier of the type to be created.
        /// @param args The arguments to be passed to the constructor.
        /// @return A pointer to the created instance.
        static T* create(const std::string& id, TArgs... args);

    private:
        /// Returns the map of registered types.
        static std::unordered_map<std::string, std::function<T*(TArgs...)>>& getConstructors();
    };

    /// Statically registers a type to be created on a factory, using the given identifier.
    /// @param factory The factory to register the type on.
    /// @param consructor The constructor to be used to create the type.
    /// @param id The identifier of the type to be registered.
#define CUBOS_REGISTER_TYPE(factory, constructor, id)                                                                  \
    namespace impl                                                                                                     \
    {                                                                                                                  \
        class factory##Entry_##id                                                                                      \
        {                                                                                                              \
        public:                                                                                                        \
            factory##Entry_##id()                                                                                      \
            {                                                                                                          \
                factory::registerType(#id, constructor);                                                               \
            }                                                                                                          \
        };                                                                                                             \
        static factory##Entry_##id factory##Entry_##id##_instance;                                                     \
    }

    // Implementation

    template <typename T, typename... TArgs>
    void Factory<T, TArgs...>::registerType(const std::string& id, std::function<T*(TArgs...)> constructor)
    {
        auto& constructors = Factory::getConstructors();
        assert(constructors.find(id) == constructors.end());
        constructors[id] = constructor;
    }

    template <typename T, typename... TArgs> T* Factory<T, TArgs...>::create(const std::string& id, TArgs... args)
    {
        auto& constructors = Factory::getConstructors();
        auto it = constructors.find(id);
        if (it == constructors.end())
        {
            logError("Factory could not create object of type '{}', which was not registered", id);
            return nullptr;
        }
        return it->second(args...);
    }

    template <typename T, typename... TArgs>
    std::unordered_map<std::string, std::function<T*(TArgs...)>>& Factory<T, TArgs...>::getConstructors()
    {
        static std::unordered_map<std::string, std::function<T*(TArgs...)>> constructors;
        return constructors;
    }

} // namespace cubos::memory

#endif // CUBOS_MEMORY_FACTORY_HPP
