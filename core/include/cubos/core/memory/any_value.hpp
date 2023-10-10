/// @file
/// @brief Class @ref cubos::core::memory::AnyValue.
/// @ingroup core-memory

#pragma once

namespace cubos::core::reflection
{
    class Type;
} // namespace cubos::core::reflection

namespace cubos::core::memory
{
    /// @brief Stores a blob of a given reflected type.
    /// @ingroup core-memory
    class AnyValue final
    {
    public:
        ~AnyValue();

        /// @brief Move constructs.
        /// @param other Value.
        AnyValue(AnyValue&& other) noexcept;

        /// @brief Default constructs a value with the given type.
        /// @note @p type must have @ref reflection::ConstructibleTrait and be default
        /// constructible.
        /// @param type Value type.
        /// @return Value.
        static AnyValue defaultConstruct(const reflection::Type& type) noexcept;

        /// @brief Copy constructs a value with the given type.
        /// @note @p type must have @ref reflection::ConstructibleTrait and be copy constructible.
        /// @param type Value type.
        /// @param value Value to copy.
        /// @return Value.
        static AnyValue copyConstruct(const reflection::Type& type, const void* value) noexcept;

        /// @brief Move constructs a value with the given type.
        /// @note @p type must have @ref reflection::ConstructibleTrait and be move constructible.
        /// @param type Value type.
        /// @param value Value to move.
        /// @return Value.
        static AnyValue moveConstruct(const reflection::Type& type, void* value) noexcept;

        /// @brief Get the type of the elements stored in the vector.
        /// @return Element type.
        const reflection::Type& type() const;

        /// @brief Gets a pointer to the underlying value.
        /// @return Value.
        void* get();

        /// @copydoc get()
        const void* get() const;

    private:
        /// @brief Constructs with allocated uninitialized memory.
        /// @warning The constructor must be called immediately after this. Will cause UB if the
        /// returned value is deconstructed before being initialized.
        /// @param type Value type.
        AnyValue(const reflection::Type& type) noexcept;

        const reflection::Type& mType;
        void* mValue;
    };
} // namespace cubos::core::memory
