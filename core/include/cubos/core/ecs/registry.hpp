#pragma once

#include <optional>

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/memory/type_map.hpp>

namespace cubos::core::ecs
{
    /// Singleton which acts as a global registry for all component types. For each component type,
    /// associated to its name, it stores a constructor for its storage type and a function for
    /// instantiating the component from a deserializer into a blueprint.
    class Registry final
    {
    public:
        Registry() = delete;

        /// Instantiates a component with the given name from a deserializer into a blueprint.
        /// @param name The name of the component.
        /// @param des The deserializer to read the component data from.
        /// @param blueprint The blueprint to instantiate the component into.
        /// @param id The id of the entity the component belongs to.
        /// @returns True if the component type was found and successfully instantiated, false otherwise.
        static bool create(std::string_view name, old::data::Deserializer& des, Blueprint& blueprint, Entity id);

        /// Instantiates a component storage for the given component type.
        /// @param type The type index of the component.
        /// @returns A pointer to the storage, or nullptr if the component type was not found.
        static std::unique_ptr<IStorage> createStorage(std::type_index type);

        /// Registers a new component type.
        /// @tparam T The component type to register.
        /// @tparam S The storage type to use for the component.
        /// @param name The name of the component.
        template <typename T, typename S>
        static void add(std::string_view name);

        /// Gets the name of a component type.
        /// @param index The type index of the component.
        /// @returns The name of the component, or std::nullopt if the component type was not found.
        static std::optional<std::string_view> name(std::type_index type);

        /// Gets the type index of a component type.
        /// @param name The name of the component.
        /// @returns The type index of the component, or std::nullopt if the component type was not found.
        static std::optional<std::type_index> type(std::string_view name);

    private:
        /// An entry in the component registry.
        struct Entry
        {
            std::type_index type; ///< Type index of the component.
            std::string name;     ///< Name of the component.

            /// Function for creating the component from a deserializer.
            bool (*componentCreator)(old::data::Deserializer&, Blueprint&, Entity);

            /// Function for creating the storage for the component.
            std::unique_ptr<IStorage> (*storageCreator)();
        };

        /// @returns The global entry registry, indexed by type.
        static memory::TypeMap<std::shared_ptr<Entry>>& entriesByType();

        /// @returns The global entry registry, indexed by name.
        static std::unordered_map<std::string, std::shared_ptr<Entry>>& entriesByName();
    };

    // Implementation.

    template <typename T, typename S>
    inline void Registry::add(std::string_view name)
    {
        static_assert(std::is_base_of_v<Storage<T>, S>, "Component storage for T must be derived from Storage<T>");

        auto& byType = Registry::entriesByType();
        auto& byNames = Registry::entriesByName();
        assert(byNames.find(std::string(name)) == byNames.end());

        auto entry = std::make_shared<Entry>(Entry{
            .type = typeid(T),
            .name = std::string(name),
            .componentCreator =
                [](old::data::Deserializer& des, Blueprint& blueprint, Entity id) {
                    T comp;
                    des.read(comp);
                    if (des.failed())
                    {
                        return false;
                    }

                    blueprint.add(id, comp);
                    return true;
                },
            .storageCreator =
                []() {
                    auto storage = std::make_unique<S>();
                    return std::unique_ptr<IStorage>(storage.release());
                },
        });

        byType.set<T>(entry);
        byNames[std::string(name)] = entry;
    }
} // namespace cubos::core::ecs

/// Macro used to register a component type.
/// An alternative to calling Registry::add() manually.
#define CUBOS_REGISTER_COMPONENT(type, storageType, name)                                                              \
    namespace cubos::core::ecs::impl                                                                                   \
    {                                                                                                                  \
        class ComponentRegister##type                                                                                  \
        {                                                                                                              \
        private:                                                                                                       \
            static inline bool registerType()                                                                          \
            {                                                                                                          \
                ::cubos::core::ecs::Registry::add<type, storageType>(name);                                            \
                return true;                                                                                           \
            }                                                                                                          \
                                                                                                                       \
            static const bool registered;                                                                              \
        };                                                                                                             \
                                                                                                                       \
        inline const bool ComponentRegister##type::registered = ComponentRegister##type::registerType();               \
    } // namespace cubos::core::ecs::impl
