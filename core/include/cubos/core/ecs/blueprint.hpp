#pragma once

#include <cubos/core/data/binary_deserializer.hpp>
#include <cubos/core/data/binary_serializer.hpp>
#include <cubos/core/data/serialization_map.hpp>
#include <cubos/core/ecs/commands.hpp>
#include <cubos/core/memory/buffer_stream.hpp>
#include <cubos/core/memory/type_map.hpp>

namespace cubos::core::ecs
{
    /// Stores a bundle of entities and their respective components, which can be easily spawned into a world.
    /// This is in a way the 'Prefab' of CUBOS., but lower level.
    class Blueprint final
    {
    public:
        Blueprint() = default;
        Blueprint(Blueprint&&) = default;
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
        template <typename... ComponentTypes>
        void add(Entity entity, const ComponentTypes&... components);

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
        friend class CommandBuffer;

        /// Stores all component data of a certain type.
        struct IBuffer
        {
            /// The names of the entities of the components present in the stream, in the same order.
            std::vector<std::string> names;
            memory::BufferStream stream; ///< Self growing buffer stream where the component data is stored.
            std::mutex mutex;            ///< Protect the stream.

            virtual ~IBuffer() = default;

            /// Adds all of the components stored in the buffer to the specified commands object.
            /// @param commands The commands object to add the components to.
            /// @param context The context to use when deserializing the components.
            virtual void addAll(CommandBuffer& commands, data::Context& context) = 0;

            /// Merges the data of another buffer of the same type into this one.
            /// @param other The buffer to merge from.
            /// @param src The context to use when serializing the components from the other buffer.
            /// @param dst The context to use when deserializing the components to this buffer.
            virtual void merge(IBuffer* other, const std::string& prefix, data::Context& src, data::Context& dst) = 0;

            /// Creates a new buffer of the same type as this one.
            virtual IBuffer* create() = 0;
        };

        /// Implementation of the IBuffer interface for a single component type.
        /// @tparam ComponentType The type of the component.
        template <typename ComponentType>
        struct Buffer : IBuffer
        {
            // Interface methods implementation.
            inline void addAll(CommandBuffer& commands, data::Context& context) override
            {
                this->mutex.lock();
                auto pos = this->stream.tell();
                this->stream.seek(0, memory::SeekOrigin::Begin);
                auto des = data::BinaryDeserializer(this->stream);
                des.context().pushSubContext(context);

                auto& map = des.context().get<data::SerializationMap<Entity, std::string>>();
                for (const auto& name : this->names)
                {
                    ComponentType type;
                    des.read(type);
                    commands.add(map.getRef(name), std::move(type));
                }
                this->stream.seek(static_cast<ptrdiff_t>(pos), memory::SeekOrigin::Begin);
                this->mutex.unlock();

                if (des.failed())
                {
                    CUBOS_CRITICAL("Could not deserialize component of type '{}'", typeid(ComponentType).name());
                    abort();
                }
            }

            inline void merge(IBuffer* other, const std::string& prefix, data::Context& src,
                              data::Context& dst) override
            {
                auto buffer = static_cast<Buffer<ComponentType>*>(other);

                buffer->mutex.lock();
                auto pos = buffer->stream.tell();
                buffer->stream.seek(0, memory::SeekOrigin::Begin);
                auto ser = data::BinarySerializer(this->stream);
                auto des = data::BinaryDeserializer(buffer->stream);
                ser.context().pushSubContext(dst);
                des.context().pushSubContext(src);
                for (const auto& name : buffer->names)
                {
                    std::string path = prefix;
                    path += '.';
                    path += name;
                    this->names.push_back(path);
                    ComponentType type;
                    des.read(type);
                    ser.write(type, "data");
                }
                buffer->stream.seek(static_cast<ptrdiff_t>(pos), memory::SeekOrigin::Begin);
                buffer->mutex.unlock();
            }

            inline IBuffer* create() override
            {
                return new Buffer<ComponentType>();
            }
        };

        /// Stores the entity handles and the associated names.
        data::SerializationMap<Entity, std::string> mMap;

        /// Buffers which store the serialized components of each type in this blueprint.
        memory::TypeMap<IBuffer*> mBuffers;
    };

    // Implementation.

    template <typename... ComponentTypes>
    Entity Blueprint::create(const std::string& name, const ComponentTypes&... components)
    {
        auto entity = Entity(static_cast<uint32_t>(mMap.size()), 0);
        mMap.add(entity, name);
        this->add(entity, components...);
        return entity;
    }

    template <typename... ComponentTypes>
    void Blueprint::add(Entity entity, const ComponentTypes&... components)
    {
        (void)entity;
        CUBOS_ASSERT(entity.generation == 0 && mMap.hasRef(entity), "Entity does not belong to blueprint");

        (
            [&]() {
                auto ptr = mBuffers.at<ComponentTypes>();
                IBuffer* buf;
                if (ptr == nullptr)
                {
                    buf = new Buffer<ComponentTypes>();
                    mBuffers.set<ComponentTypes>(buf);
                }
                else
                {
                    buf = *ptr;
                }

                auto ser = data::BinarySerializer(buf->stream);
                ser.context().push(mMap);
                ser.write(components, "data");
                buf->names.push_back(mMap.getId(entity));
            }(),

            ...);
    }
} // namespace cubos::core::ecs
