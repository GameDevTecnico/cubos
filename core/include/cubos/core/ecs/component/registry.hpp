/// @file
/// @brief Class @ref cubos::core::ecs::Registry and registration macro.
/// @ingroup core-ecs-component

#pragma once

#include <optional>

#include <cubos/core/data/old/deserializer.hpp>
#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/memory/type_map.hpp>

namespace cubos::core::ecs
{
    /// @brief Singleton which holds a global registry for all component types.
    ///
    /// It stores information regarding the components so that they can be handled in a type-erased
    /// manner.
    ///
    /// @ingroup core-ecs-component
    class Registry final
    {
    public:
        /// @brief Deleted constructor.
        Registry() = delete;

        /// @brief Instantiates a component with the given name from a deserializer into a blueprint.
        /// @param name Name of the component.
        /// @param des Deserializer to read the component data from.
        /// @param blueprint Blueprint to instantiate the component into.
        /// @param id Blueprint entity to add the component to.
        /// @return Whether the instantiation was successful.
        static bool create(std::string_view name, data::old::Deserializer& des, Blueprint& blueprint, Entity id);

        /// @brief Instantiates a component storage for the given component type.
        /// @param type Type index of the component.
        /// @return Smart pointer to the storage, or nullptr if the component type was not found.
        static std::unique_ptr<IStorage> createStorage(std::type_index type);

        /// @brief Registers a new component type.
        /// @tparam T Component type to register.
        /// @tparam S Storage type to use for the component.
        /// @param name Name of the component.
        template <typename T, typename S>
        static void add(std::string_view name);

        /// @brief Gets the name of a component type.
        /// @param type Type index of the component.
        /// @return Name of the component, or std::nullopt if the component type was not found.
        static std::optional<std::string_view> name(std::type_index type);

        /// @brief Gets the type index of a component type.
        /// @param name Name of the component.
        /// @return Type index of the component, or std::nullopt if the component type was not
        /// found.
        static std::optional<std::type_index> type(std::string_view name);

    private:
        /// @brief Entry in the component registry.
        struct Entry
        {
            std::type_index type; ///< Type index of the component.
            std::string name;     ///< Name of the component.

            /// Function for creating the component from a deserializer.
            bool (*componentCreator)(data::old::Deserializer&, Blueprint&, Entity);

            /// Function for creating the storage for the component.
            std::unique_ptr<IStorage> (*storageCreator)();
        };

        /// @return Global entry registry, indexed by type.
        static memory::TypeMap<std::shared_ptr<Entry>>& entriesByType();

        /// @return Global entry registry, indexed by name.
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
                [](data::old::Deserializer& des, Blueprint& blueprint, Entity id) {
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

/// @brief Macro used to register a component type as an alternative to calling
/// @ref cubos::core::ecs::Registry::add() manually.
/// @warning Care must be taken to ensure that this macro is included from at least one translation
/// unit (think of it as a .cpp file) in the executable, otherwise the component type may not be
/// registered. E.g.: keeping it hidden in a .cpp file on a library may not work.
/// @ingroup core-ecs-component
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
