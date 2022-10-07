#ifndef CUBOS_CORE_ECS_BLUEPRINT_HPP
#define CUBOS_CORE_ECS_BLUEPRINT_HPP

#include <cubos/core/ecs/commands.hpp>

#include <cubos/core/data/binary_serializer.hpp>
#include <cubos/core/data/binary_deserializer.hpp>
#include <cubos/core/data/serialization_map.hpp>

namespace cubos::core::ecs
{
    /// Stores a bundle of entities and their respective components, which can be easily spawned into a world.
    /// This is in a way the 'Prefab' of CUBOS., but lower level.
    class Blueprint final
    {
    public:
        Blueprint() = default;
        ~Blueprint();

        /// Creates a new entity and returns its identifier.
        /// @tparam ComponentTypes The types of components to add to the entity.
        /// @param name The name of the entity.
        /// @param components The components to add to the entity.
        /// @returns The new entity.
        template <typename... ComponentTypes>
        Entity create(const std::string& name, const ComponentTypes&... components);

        /// Adds components to an entity.
        /// @tparam ComponentTypes The types of the components.
        /// @param entity The entity to add the components to.
        /// @param components The components to add.
        template <typename... ComponentTypes> void add(Entity entity, const ComponentTypes&... components);

        /// Deserializes a component and adds it to an entity.
        /// @param entity The entity to add the component to.
        /// @param name The name of the component type.
        /// @param deserializer The deserializer to deserialize from.
        bool addFromDeserializer(Entity entity, const std::string& name, data::Deserializer& deserializer);

        /// Returns an entity from its name.
        /// @param name The name of the entity.
        /// @returns The entity, which may be 'none' if the entity doesn't exist (validate with entity.isNone).
        Entity entity(const std::string& name) const;

        /// Merges another blueprint into this one.
        /// All of the names of the other blueprint will be prefixed with the specified string.
        /// @param prefix The name to prefix with the merged blueprint.
        /// @param other The other blueprint to merge.
        void merge(const std::string& prefix, const Blueprint& other);

        /// Clears the blueprint, removing any added entities and components.
        void clear();

    private:
        friend class Commands;

        /// Stores all component data of a certain type.
        struct IBuffer
        {
            std::vector<std::string>
                names; ///< The names of the entities of the components present in the stream, in the same order.
            memory::BufferStream stream; ///< Self growing buffer stream where the component data is stored.
            std::mutex mutex;            ///< Protect the stream.

            /// Adds all of the components stored in the buffer to the specified commands object.
            /// @param commands The commands object to add the components to.
            /// @param map The serialization map which maps from the entity names to the instantiated entities.
            virtual void addAll(Commands& commands, const data::SerializationMap<Entity, std::string>& map) = 0;

            /// Merges the data of another buffer of the same type into this one.
            /// @param other The buffer to merge from.
            /// @param src The serialization map which maps the other entity names to the entites of the other buffer.
            /// @param dst The serialization map which maps the entity names to the entities of this buffer.
            virtual void merge(IBuffer* other, const std::string& prefix,
                               const data::SerializationMap<Entity, std::string>& src,
                               const data::SerializationMap<Entity, std::string>& dst) = 0;

            /// Creates a new buffer of the same type as this one.
            virtual IBuffer* create() = 0;
        };

        /// Implementation of the IBuffer interface for a single component type.
        /// @tparam ComponentType The type of the component.
        template <typename ComponentType> struct Buffer : IBuffer
        {
            // Interface methods implementation.

            void addAll(Commands& commands, const data::SerializationMap<Entity, std::string>& map) override;
            virtual void merge(IBuffer* other, const std::string& prefix,
                               const data::SerializationMap<Entity, std::string>& src,
                               const data::SerializationMap<Entity, std::string>& dst) override;
            IBuffer* create() override;
        };

        /// Map used to serialize entities, which maps the entities in the blueprint to their names.
        data::SerializationMap<Entity, std::string> map;

        /// Buffers which store the serialized components of each type in this blueprint.
        std::unordered_map<std::type_index, IBuffer*> buffers;
    };

    // Implementation.

    template <typename... ComponentTypes>
    Entity Blueprint::create(const std::string& name, const ComponentTypes&... components)
    {
        auto entity = Entity(static_cast<uint32_t>(this->map.size()), 0);
        this->map.add(entity, name);
        this->add(entity, components...);
        return entity;
    }

    template <typename... ComponentTypes> void Blueprint::add(Entity entity, const ComponentTypes&... components)
    {
        assert(entity.generation == 0 && this->map.hasRef(entity));

        (
            [&]() {
                auto it = this->buffers.find(typeid(ComponentTypes));
                if (it == this->buffers.end())
                {
                    it = this->buffers.emplace(typeid(ComponentTypes), new Buffer<ComponentTypes>()).first;
                }

                data::BinarySerializer(it->second->stream).write(components, &this->map, "data");
                it->second->names.push_back(this->map.getId(entity));
            }(),

            ...);
    }

    template <typename ComponentType>
    void Blueprint::Buffer<ComponentType>::addAll(Commands& commands,
                                                  const data::SerializationMap<Entity, std::string>& map)
    {
        this->mutex.lock();
        auto pos = this->stream.tell();
        this->stream.seek(0, memory::SeekOrigin::Begin);
        auto des = data::BinaryDeserializer(this->stream);
        for (auto name : this->names)
        {
            ComponentType type;
            des.read(type, &map);
            commands.add(map.getRef(name), std::move(type));
        }
        this->stream.seek(pos, memory::SeekOrigin::Begin);
        this->mutex.unlock();

        if (des.failed())
        {
            logCritical("Blueprint::Buffer::addAll() failed: deserialization of component type '{}' failed.",
                        typeid(ComponentType).name());
            abort();
        }
    }

    template <typename ComponentType>
    void Blueprint::Buffer<ComponentType>::merge(IBuffer* other, const std::string& prefix,
                                                 const data::SerializationMap<Entity, std::string>& src,
                                                 const data::SerializationMap<Entity, std::string>& dst)
    {
        auto buffer = static_cast<Buffer<ComponentType>*>(other);

        buffer->mutex.lock();
        auto pos = buffer->stream.tell();
        buffer->stream.seek(0, memory::SeekOrigin::Begin);
        auto ser = data::BinarySerializer(this->stream);
        auto des = data::BinaryDeserializer(buffer->stream);
        for (auto name : buffer->names)
        {
            this->names.push_back(prefix + "." + name);
            ComponentType type;
            des.read(type, &src);
            ser.write(type, &dst, "data");
        }
        buffer->stream.seek(pos, memory::SeekOrigin::Begin);
        buffer->mutex.unlock();
    }

    template <typename ComponentType> Blueprint::IBuffer* Blueprint::Buffer<ComponentType>::create()
    {
        return new Buffer<ComponentType>();
    }
} // namespace cubos::core::ecs

#endif // CUBOS_CORE_ECS_BLUEPRINT_HPP
