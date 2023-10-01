/// @file
/// @brief Class @ref cubos::core::ecs::Blueprint.
/// @ingroup core-ecs

#pragma once

#include <cubos/core/data/old/binary_deserializer.hpp>
#include <cubos/core/data/old/binary_serializer.hpp>
#include <cubos/core/data/old/serialization_map.hpp>
#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/ecs/system/commands.hpp>
#include <cubos/core/memory/buffer_stream.hpp>
#include <cubos/core/memory/type_map.hpp>

namespace cubos::core::ecs
{
    /// @brief Stores a bundle of entities and their respective components, which can be easily
    /// spawned into a world. This is in a way the 'Prefab' of @b CUBOS., but lower level.
    class Blueprint final
    {
    public:
        ~Blueprint();

        /// @brief Constructs an empty blueprint.
        Blueprint() = default;

        /// @brief Move constructs.
        Blueprint(Blueprint&&) = default;

        /// @brief Creates a new entity and returns its identifier.
        /// @tparam ComponentTypes Component types.
        /// @param name Entity name.
        /// @param components Components to add.
        /// @return Entity identifier.
        template <typename... ComponentTypes>
        Entity create(const std::string& name, const ComponentTypes&... components);

        /// @brief Adds components to an entity.
        /// @tparam ComponentTypes Component types.
        /// @param entity Entity identifier.
        /// @param components Components to add.
        template <typename... ComponentTypes>
        void add(Entity entity, const ComponentTypes&... components);

        /// @brief Deserializes a component and adds it to an entity.
        /// @param entity Entity identifier.
        /// @param name Component type name.
        /// @param deserializer Ddeserializer to deserialize from.
        bool addFromDeserializer(Entity entity, const std::string& name, data::old::Deserializer& deserializer);

        /// @brief Returns an entity from its name.
        /// @param name Entity name.
        /// @return Entity identifier, or null entity if not found.
        Entity entity(const std::string& name) const;

        /// @brief Merges another blueprint into this one.
        /// @note All of the entity names of the other blueprint will be prefixed with the
        /// specified string.
        /// @param prefix Name to prefix with the merged blueprint.
        /// @param other Other blueprint to merge.
        void merge(const std::string& prefix, const Blueprint& other);

        /// @brief Clears the blueprint, removing any added entities and components.
        void clear();

        /// @brief Returns the internal map that maps entities to their names
        /// @return Map of entities and names.
        inline std::unordered_map<Entity, std::string, EntityHash> getMap() const
        {
            return mMap.getMap();
        }

    private:
        friend class CommandBuffer;

        /// @brief Stores all component data of a certain type.
        struct IBuffer
        {
            /// @brief Names of the entities of the components present in the stream, in the same order.
            std::vector<std::string> names;
            memory::BufferStream stream; ///< Self growing buffer stream where the component data is stored.
            std::mutex mutex;            ///< Protect the stream.

            virtual ~IBuffer() = default;

            /// @brief Adds all of the components stored in the buffer to the specified commands object.
            /// @param commands Commands object to add the components to.
            /// @param context Context to use when deserializing the components.
            virtual void addAll(CommandBuffer& commands, data::old::Context& context) = 0;

            /// @brief Merges the data of another buffer of the same type into this one.
            /// @param other Buffer to merge from.
            /// @param prefix Prefix to add to the names of the components of the other buffer.
            /// @param src Context to use when serializing the components from the other buffer.
            /// @param dst Context to use when deserializing the components to this buffer.
            virtual void merge(IBuffer* other, const std::string& prefix, data::old::Context& src,
                               data::old::Context& dst) = 0;

            /// @brief Creates a new buffer of the same type as this one.
            /// @return New buffer.
            virtual IBuffer* create() = 0;
        };

        /// @brief Implementation of the IBuffer interface for the component type @p ComponentType.
        /// @tparam ComponentType Component type.
        template <typename ComponentType>
        struct Buffer : IBuffer
        {
            // Interface methods implementation.

            inline void addAll(CommandBuffer& commands, data::old::Context& context) override
            {
                this->mutex.lock();
                auto pos = this->stream.tell();
                this->stream.seek(0, memory::SeekOrigin::Begin);
                auto des = data::old::BinaryDeserializer(this->stream);
                des.context().pushSubContext(context);

                auto& map = des.context().get<data::old::SerializationMap<Entity, std::string, EntityHash>>();
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

            inline void merge(IBuffer* other, const std::string& prefix, data::old::Context& src,
                              data::old::Context& dst) override
            {
                auto buffer = static_cast<Buffer<ComponentType>*>(other);

                buffer->mutex.lock();
                auto pos = buffer->stream.tell();
                buffer->stream.seek(0, memory::SeekOrigin::Begin);
                auto ser = data::old::BinarySerializer(this->stream);
                auto des = data::old::BinaryDeserializer(buffer->stream);
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

        /// @brief Stores the entity handles and the associated names.
        data::old::SerializationMap<Entity, std::string, EntityHash> mMap;

        /// @brief Buffers which store the serialized components of each type in this blueprint.
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

                auto ser = data::old::BinarySerializer(buf->stream);
                ser.context().push(mMap);
                ser.write(components, "data");
                buf->names.push_back(mMap.getId(entity));
            }(),

            ...);
    }
} // namespace cubos::core::ecs
