#ifndef CUBOS_CORE_ECS_REGISTRY_HPP
#define CUBOS_CORE_ECS_REGISTRY_HPP

#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/memory/type_map.hpp>

namespace cubos::core::ecs
{
    /// Singleton which stores the global registry of component types, allowing components to be created from
    /// deserializers.
    class Registry final
    {
    public:
        Registry() = delete;

        /// Instantiates a new component into a blueprint from a deserializer.
        /// @param name The name of the component.
        /// @param des The deserializer to read the component data from.
        /// @param blueprint The blueprint to instantiate the component into.
        /// @param id The id of the entity the component belongs to.
        /// @returns True if the component type was found and successfully instantiated, false otherwise.
        static bool create(const std::string& name, data::Deserializer& des, Blueprint& blueprint, Entity id);

        /// Registers a new component type.
        /// @tparam T The component type to register.
        /// @param name The name of the component.
        template <typename ComponentType>
        static void add(const std::string& name);

        /// Gets the name of a component type.
        /// @param index The type index of the component.
        /// @returns The name of the component.
        static const std::string& name(std::type_index index);

        /// Gets the type index of a component type.
        /// @param name The name of the component.
        /// @returns The type index of the component, or typeid(void) if the component type was not found.
        static std::type_index index(const std::string& name);

    private:
        /// Function type for creating components from deserializers.
        using Creator = std::function<bool(data::Deserializer&, Blueprint&, Entity)>;

        /// Accesses the global component creator registry.
        static std::unordered_map<std::string, Creator>& creators();

        /// Accesses the global component name registry.
        static memory::TypeMap<std::string>& names();
    };

    // Implementation.

    template <typename ComponentType>
    void Registry::add(const std::string& name)
    {
        auto& creators = Registry::creators();
        auto& names = Registry::names();
        assert(creators.find(name) == creators.end());

        creators.emplace(name, [](data::Deserializer& des, Blueprint& blueprint, Entity id) {
            ComponentType comp;
            des.read(comp);
            if (des.failed())
            {
                return false;
            }
            else
            {
                blueprint.add(id, comp);
                return true;
            }
        });

        names.set<ComponentType>(name);
    }
} // namespace cubos::core::ecs

/// Macro used to register a component type so that it can be deserialized from files.
/// An alternative to calling Registry::add() manually.
#define CUBOS_REGISTER_COMPONENT(type, name)                                                                           \
    namespace cubos::core::ecs::impl                                                                                   \
    {                                                                                                                  \
        class ComponentRegister##type                                                                                  \
        {                                                                                                              \
        private:                                                                                                       \
            static inline bool registerType()                                                                          \
            {                                                                                                          \
                ::cubos::core::ecs::Registry::add<type>(name);                                                         \
                return true;                                                                                           \
            }                                                                                                          \
                                                                                                                       \
            static const bool registered;                                                                              \
        };                                                                                                             \
                                                                                                                       \
        inline const bool ComponentRegister##type::registered = ComponentRegister##type::registerType();               \
    } // namespace cubos::core::ecs::impl

#endif // CUBOS_CORE_ECS_REGISTRY_HPP
