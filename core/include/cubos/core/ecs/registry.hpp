#ifndef CUBOS_CORE_ECS_REGISTRY_HPP
#define CUBOS_CORE_ECS_REGISTRY_HPP

#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/data/deserializer.hpp>

namespace cubos::core::ecs
{
    /// Singleton which stores the global registry of component types, allowing components to be created from
    /// deserializers.
    class Registry final
    {
    public:
        /// Function type for creating components from deserializers.
        using Creator =
            std::function<bool(data::Deserializer&, Blueprint&, data::SerializationMap<Entity, std::string>*, Entity)>;

        Registry() = delete;

        /// Instantiates a new component into a blueprint from a deserializer.
        /// @param name The name of the component.
        /// @param des The deserializer to read the component data from.
        /// @param blueprint The blueprint to instantiate the component into.
        /// @param id The id of the entity the component belongs to.
        /// @returns True if the component type was found and successfully instantiated, false otherwise.
        static bool create(const std::string& name, data::Deserializer& des, Blueprint& blueprint,
                           data::SerializationMap<Entity, std::string>* map, Entity id);

        /// Registers a component type described by a deserializer and a name.
        /// @param index The type index of the component.
        /// @param name The name of the component.
        /// @param fun The function to instantiate the component into a buffer.
        static void add(std::type_index index, const std::string& name, Creator fun);

        /// Gets the name of a component type.
        /// @param index The type index of the component.
        /// @returns The name of the component.
        static const std::string& name(std::type_index index);

    private:
        /// Accesses the global component creator registry.
        static std::unordered_map<std::string, Creator>& creators();

        /// Accesses the global component name registry.
        static std::unordered_map<std::type_index, std::string>& names();
    };
} // namespace cubos::core::ecs

/// Macro used to register a component type so that it can be deserialized from files.
#define CUBOS_REGISTER_COMPONENT(type, name)                                                                           \
    namespace cubos::core::ecs::impl                                                                                   \
    {                                                                                                                  \
        class ComponentRegister##type                                                                                  \
        {                                                                                                              \
        private:                                                                                                       \
            static inline bool registerType()                                                                          \
            {                                                                                                          \
                ::cubos::core::ecs::Registry::add(                                                                     \
                    typeid(type), name,                                                                                \
                    [](::cubos::core::data::Deserializer& des, ::cubos::core::ecs::Blueprint& blueprint,               \
                       ::cubos::core::data::SerializationMap<Entity, std::string>* map, Entity id) {                   \
                        type comp;                                                                                     \
                        des.read(comp, map);                                                                           \
                        if (des.failed())                                                                              \
                        {                                                                                              \
                            return false;                                                                              \
                        }                                                                                              \
                        else                                                                                           \
                        {                                                                                              \
                            blueprint.add(id, comp);                                                                   \
                            return true;                                                                               \
                        }                                                                                              \
                    });                                                                                                \
                return true;                                                                                           \
            }                                                                                                          \
                                                                                                                       \
            static const bool registered;                                                                              \
        };                                                                                                             \
                                                                                                                       \
        inline const bool ComponentRegister##type::registered = ComponentRegister##type::registerType();               \
    } // namespace cubos::core::ecs::impl

#endif // CUBOS_CORE_ECS_REGISTRY_HPP
