/// @file
/// @brief Class @ref cubos::core::data::Deserializer.
/// @ingroup core-data-des

#pragma once

#include <unordered_map>

#include <cubos/core/memory/function.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::data
{
    /// @brief Base class for deserializers, which defines the interface for deserializing
    /// arbitrary data using its reflection metadata.
    ///
    /// Deserializers are type visitors which allow overriding the default deserialization
    /// behaviour for each type using hooks. Hooks are functions which are called when the
    /// deserializer encounters a type, and can be used to customize the deserialization process.
    ///
    /// If a type which can't be further decomposed is encountered for which no hook is defined,
    /// the deserializer will emit a warning and fail. Implementations should set default hooks for
    /// at least the primitive types.
    ///
    /// @ingroup core-data-des
    class CUBOS_CORE_API Deserializer
    {
    public:
        virtual ~Deserializer() = default;

        /// @brief Constructs.
        Deserializer() = default;

        /// @name Deleted copy and move constructors.
        /// @brief Deleted as the hooks may contain references to the deserializer.
        /// @{
        Deserializer(Deserializer&&) = delete;
        Deserializer(const Deserializer&) = delete;
        /// @}

        /// @brief Function type for deserialization hooks.
        using Hook = memory::Function<bool(void*)>;

        /// @brief Function type for deserialization hooks.
        /// @tparam T Type.
        template <typename T>
        using TypedHook = memory::Function<bool(T&)>;

        /// @brief Deserialize the given value.
        /// @param type Type.
        /// @param value Value.
        /// @return Whether the value was successfully deserialized.
        bool read(const reflection::Type& type, void* value);

        /// @brief Deserialize the given value.
        /// @tparam T Type.
        /// @param value Value.
        /// @return Whether the value was successfully deserialized.
        template <typename T>
        bool read(T& value)
        {
            return this->read(reflection::reflect<T>(), &value);
        }

        /// @brief Sets the hook to be called on deserialization of the given type.
        /// @param type Type.
        /// @param hook Hook.
        void hook(const reflection::Type& type, Hook hook);

        /// @brief Sets the hook to be called on deserialization of the given type.
        /// @tparam T Type.
        /// @param hook Hook.
        template <typename T>
        void hook(TypedHook<T> hook)
        {
            this->hook(reflection::reflect<T>(),
                       [hook = memory::move(hook)](void* value) mutable { return hook(*static_cast<T*>(value)); });
        }

    protected:
        /// @brief Called for each type with no hook defined.
        ///
        /// Should recurse by calling @ref read() again as appropriate.
        ///
        /// @param type Type.
        /// @param value Value.
        /// @return Whether the value was successfully deserialized.
        virtual bool decompose(const reflection::Type& type, void* value) = 0;

    private:
        std::unordered_map<const reflection::Type*, Hook> mHooks;
    };
} // namespace cubos::core::data
