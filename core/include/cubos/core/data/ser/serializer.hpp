/// @file
/// @brief Defines the Serializer abstract class.

#pragma once

#include <functional>
#include <unordered_map>

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/visitor.hpp>

namespace cubos::core::reflection
{
    class Type;
} // namespace cubos::core::reflection

namespace cubos::core::data
{
    /// @brief Base class for serializers, which defines the interface for serializing arbitrary
    /// data using its reflection metadata.
    ///
    /// @details Serializers are type visitors which allow overriding the default behavior for
    /// serializing each type using hooks. Hooks are functions which are called when the serializer
    /// encounters a type, and can be used to customize the serialization process.
    ///
    /// If a primitive type is encountered for which no hook is defined, the serializer will abort
    /// with an error. Usually, implementations should set default hooks for the most common
    /// primitive types.
    ///
    /// @see JSONSerializer
    class Serializer : private reflection::TypeVisitor
    {
    public:
        /// @brief Function type for serialization hooks.
        using AnyHook = std::function<void(const void*)>;

        /// @brief Function type for serialization hooks.
        /// @tparam T Type of the data to serialize.
        template <typename T>
        using Hook = std::function<void(const T&)>;

        Serializer() = default;

        /// @name Forbid copying and moving.
        /// @brief Necessary since hooks may capture a pointer to the serializer.
        /// @{
        Serializer(const Serializer&) = delete;
        Serializer(Serializer&&) = delete;
        Serializer& operator=(const Serializer&) = delete;
        Serializer& operator=(Serializer&&) = delete;
        /// @}

        /// @brief Serialize the given data.
        /// @param type Type of the data to serialize.
        /// @param data Pointer to an instance of the given type.
        void write(const reflection::Type& type, const void* data);

        /// @brief Serialize the given data.
        /// @tparam T Type of the data to serialize.
        /// @param data Reference to an instance of the given type.
        template <typename T>
        void write(const T& data)
        {
            this->write(reflection::reflect<T>(), &data);
        }

        /// @brief Sets the hook to be called on serialization of the given type.
        /// @param type Type to set the hook for.
        /// @param hook Hook to call.
        void hook(const reflection::Type& type, AnyHook hook);

        /// @brief Sets the hook to be called on serialization of the given type.
        /// @tparam T Type to set the hook for.
        /// @param hook Hook to call.
        template <typename T>
        void hook(Hook<T> hook)
        {
            this->hook(reflection::reflect<T>(), [hook](const void* data) { hook(*static_cast<const T*>(data)); });
        }

    protected:
        /// @brief Serializes the given object.
        /// @param type Object type.
        /// @param data Pointer to an instance of the given type.
        virtual void writeObject(const reflection::ObjectType& type, const void* data) = 0;

        /// @brief Serializes the given variant.
        /// @param type Variant type.
        /// @param data Pointer to an instance of the given type.
        virtual void writeVariant(const reflection::VariantType& type, const void* data) = 0;

        /// @brief Serializes the given array.
        /// @param type Array type.
        /// @param data Pointer to an instance of the given type.
        virtual void writeArray(const reflection::ArrayType& type, const void* data) = 0;

        /// @brief Serializes the given dictionary.
        /// @param type Dictionary type.
        /// @param data Pointer to an instance of the given type.
        virtual void writeDictionary(const reflection::DictionaryType& type, const void* data) = 0;

    private:
        void visit(const reflection::PrimitiveType& type) override;
        void visit(const reflection::ObjectType& type) override;
        void visit(const reflection::VariantType& type) override;
        void visit(const reflection::ArrayType& type) override;
        void visit(const reflection::DictionaryType& type) override;

        std::unordered_map<const reflection::Type*, AnyHook> mHooks;
        const void* mInstance;
    };
} // namespace cubos::core::data
