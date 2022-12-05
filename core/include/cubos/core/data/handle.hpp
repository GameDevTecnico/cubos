#ifndef CUBOS_CORE_DATA_HANDLE_HPP
#define CUBOS_CORE_DATA_HANDLE_HPP

#include <cubos/core/data/serializer.hpp>
#include <cubos/core/data/deserializer.hpp>

#include <string>

namespace cubos::core::data
{
    /// A generic handle to a resource.
    class Handle
    {
    public:
        using SerContext = std::function<void(Serializer&, const Handle&, const char*)>;
        using DesContext = std::function<void(Deserializer&, Handle&)>;

        Handle(const char* type, size_t* refCount, void* data, const std::string* id);
        Handle(std::nullptr_t p = nullptr);
        Handle(const Handle& rhs);
        Handle(Handle&& rhs);
        virtual ~Handle();

        operator bool() const;
        Handle& operator=(const Handle& rhs);

        /// Checks if the handle is valid.
        bool isValid() const;

        /// Gets a raw pointer to the resource referenced by this handle.
        const void* getRaw() const;

        /// Gets the id of the resource referenced by this handle.
        const std::string& getId() const;

    protected:
        const char* type;
        size_t* refCount;
        void* data;
        const std::string* id;
    };

    /// Serializes the handle to the given serializer.
    /// @param serializer The serializer to use.
    /// @param handle The handle to serialize.
    /// @param ctx The serialization context.
    /// @param name The name of the handle.
    void serialize(Serializer& serializer, const Handle& handle, Handle::SerContext ctx, const char* name);

    /// Deserializes the handle from the given deserializer.
    /// @param deserializer The deserializer to use.
    /// @param handle The handle to deserialize.
    /// @param ctx The deserialization context.
    void deserialize(Deserializer& deserializer, Handle& handle, Handle::DesContext ctx);
}; // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_HANDLE_HPP
