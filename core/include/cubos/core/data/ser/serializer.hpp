/// @file
/// @brief Class @ref cubos::core::data::Serializer.
/// @ingroup core-data-ser

#pragma once

#include <unordered_map>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::data
{
    /// @brief Base class for serializers, which defines the interface for serializing arbitrary
    /// data using its reflection metadata.
    ///
    /// Serializers are type visitors which allow overriding the default serialization behaviour
    /// for each type using hooks. Hooks are functions which are called when the serializer
    /// encounters a type, and can be used to customize the serialization process.
    ///
    /// If a type which can't be further decomposed is encountered for which no hook is defined,
    /// the serializer will emit a warning and fail. Implementations should set default hooks for
    /// at least the primitive types.
    ///
    /// @ingroup core-data-ser
    class Serializer
    {
    public:
        virtual ~Serializer() = default;

        /// @brief Function type for serialization hooks.
        /// @param ser Serializer.
        /// @param type Type.
        /// @param value Value.
        /// @return Whether the value was successfully serialized.
        using Hook = bool (*)(Serializer& ser, const reflection::Type& type, const void* value);

        /// @brief Serialize the given value.
        /// @param type Type.
        /// @param value Value.
        /// @return Whether the value was successfully serialized.
        bool write(const reflection::Type& type, const void* value);

        /// @brief Serialize the given value.
        /// @tparam T Type.
        /// @param value Value.
        /// @return Whether the value was successfully serialized.
        template <typename T>
        bool write(const T& value)
        {
            return this->write(reflection::reflect<T>(), &value);
        }

        /// @brief Sets the hook to be called on serialization of the given type.
        /// @param type Type.
        /// @param hook Hook.
        void hook(const reflection::Type& type, Hook hook);

        /// @brief Sets the hook to be called on serialization of the given type.
        /// @tparam T Type.
        /// @param hook Hook.
        template <typename T>
        void hook(Hook hook)
        {
            this->hook(reflection::reflect<T>(), hook);
        }

    protected:
        /// @brief Called for each type with no hook defined.
        ///
        /// Should recurse by calling @ref write() again as appropriate.
        ///
        /// @param type Type.
        /// @param value Value.
        /// @return Whether the value was successfully serialized.
        virtual bool decompose(const reflection::Type& type, const void* value) = 0;

    private:
        std::unordered_map<const reflection::Type*, Hook> mHooks;
    };
} // namespace cubos::core::data
