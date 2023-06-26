/// @file
/// @brief Defines the Deserializer abstract class.

#pragma once

#include <functional>
#include <unordered_map>

#include <cubos/core/reflection/type.hpp>
#include <cubos/core/reflection/visitor.hpp>

namespace cubos::core::reflection
{
    class Type;
} // namespace cubos::core::reflection

namespace cubos::core::data
{
    /// @brief Base class for deserializers, which defines the interface for deserializing
    /// arbitrary data using its reflection metadata.
    ///
    /// @details Deserializers are type visitors which allow overriding the default behavior for
    /// deserializing each type using hooks. Hooks are functions which are called when the
    /// deserializer encounters a type, and can be used to customize the deserialization process.
    ///
    /// If a primitive type is encountered for which no hook is defined, the deserializer will
    /// abort with an error. Usually, implementations should set default hooks for the most common
    /// primitive types.
    ///
    /// @see JSONDeserializer
    class Deserializer : private reflection::TypeVisitor
    {
    public:
        /// @brief Function type for deserialization hooks.
        using AnyHook = std::function<bool(void*)>;

        /// @brief Function type for deserialization hooks.
        /// @tparam T Type of the data to deserialize.
        template <typename T>
        using Hook = std::function<bool(T&)>;

        Deserializer() = default;

        /// @name Forbid copying and moving.
        /// @brief Necessary since hooks may capture a pointer to the deserializer.
        /// @{
        Deserializer(const Deserializer&) = delete;
        Deserializer(Deserializer&&) = delete;
        Deserializer& operator=(const Deserializer&) = delete;
        Deserializer& operator=(Deserializer&&) = delete;
        /// @}

        /// @brief Deserialize the given data. If the operation fails, the data is left on an
        /// unspecified state - it may be partially deserialized, or not deserialized at all.
        /// @param type Type of the data to deserialize.
        /// @param data Pointer to an instance of the given type.
        /// @returns Whether the data was successfully deserialized.
        bool read(const reflection::Type& type, void* data);

        /// @brief Deserialize the given data. If the operation fails, the data is left on an
        /// unspecified state - it may be partially deserialized, or not deserialized at all.
        /// @tparam T Type of the data to deserialize.
        /// @param data Pointer to an instance of the given type.
        /// @returns Whether the data was successfully deserialized.
        template <typename T>
        bool read(T& data)
        {
            return this->read(reflection::reflect<T>(), &data);
        }

        /// @brief Sets the hook to be called on deserialization of the given type.
        /// @param type Type to set the hook for.
        /// @param hook Hook to call.
        void hook(const reflection::Type& type, AnyHook hook);

        /// @brief Sets the hook to be called on deserialization of the given type.
        /// @tparam T Type to set the hook for.
        /// @param hook Hook to call.
        template <typename T>
        void hook(Hook<T> hook)
        {
            this->hook(reflection::reflect<T>(), [hook](void* data) { return hook(*static_cast<T*>(data)); });
        }

    protected:
        /// @brief Deserializes the given object.
        /// @param type Object type.
        /// @param data Pointer to an instance of the given type.
        /// @returns Whether the object was successfully deserialized.
        virtual bool readObject(const reflection::ObjectType& type, void* data) = 0;

        /// @brief Deserializes the given variant.
        /// @param type Variant type.
        /// @param data Pointer to an instance of the given type.
        /// @returns Whether the variant was successfully deserialized.
        virtual bool readVariant(const reflection::VariantType& type, void* data) = 0;

        /// @brief Deserializes the given array.
        /// @param type Array type.
        /// @param data Pointer to an instance of the given type.
        /// @returns Whether the array was successfully deserialized.
        virtual bool readArray(const reflection::ArrayType& type, void* data) = 0;

        /// @brief Deserializes the given dictionary.
        /// @param type Dictionary type.
        /// @param data Pointer to an instance of the given type.
        /// @returns Whether the dictionary was successfully deserialized.
        virtual bool readDictionary(const reflection::DictionaryType& type, void* data) = 0;

    private:
        void visit(const reflection::PrimitiveType& type) override;
        void visit(const reflection::ObjectType& type) override;
        void visit(const reflection::VariantType& type) override;
        void visit(const reflection::ArrayType& type) override;
        void visit(const reflection::DictionaryType& type) override;

        std::unordered_map<const reflection::Type*, AnyHook> mHooks;
        void* mInstance;
    };
} // namespace cubos::core::data
