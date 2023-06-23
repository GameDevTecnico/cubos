/// @file
/// @brief Defines the Serializer abstract class.

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
    /// @brief Base class for serializers, which defines the interface for serializing arbitrary
    /// data using its reflection metadata.
    class Serializer : private reflection::TypeVisitor
    {
    public:
        /// @brief Function type for serialization hooks.
        using Hook = std::function<void(const void*)>;

        Serializer() = default;

        /// @name Forbid copy and move constructors.
        /// @{
        Serializer(const Serializer&) = delete;
        Serializer(Serializer&&) = delete;
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
        void hook(const reflection::Type& type, Hook hook);

        /// @brief Sets the hook to be called on serialization of the given type.
        /// @tparam T Type to set the hook for.
        /// @param hook Hook to call.
        template <typename T>
        void hook(Hook hook)
        {
            this->hook(reflection::reflect<T>(), hook);
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

        std::unordered_map<const reflection::Type*, Hook> mHooks;
        const void* mInstance;
    };
} // namespace cubos::core::data
