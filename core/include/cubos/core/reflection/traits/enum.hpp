/// @file
/// @brief Class @ref cubos::core::reflection::EnumTrait.
/// @ingroup core-reflection

#pragma once

#include <cstdint>
#include <string>

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/type.hpp>

namespace cubos::core::reflection
{
    /// @brief Provides enumeration functionality to an enumerated type.
    /// @see See @ref examples-core-reflection-traits-enum for an example of using this trait.
    /// @ingroup core-reflection
    class EnumTrait
    {
    public:
        /// @brief Represents an individual variant within the enumerated type.
        class Variant;

        /// @brief Used to iterate over the variants on an enum trait.
        class Iterator;

        /// @brief Function pointer to test if a value matches the variant.
        using Tester = bool (*)(const void* instance);

        /// @brief Function pointer to set the value of the variant.
        using Setter = void (*)(void* instance);

        ~EnumTrait();

        /// @brief Constructs.
        EnumTrait() = default;

        /// @brief Move constructs.
        /// @param other Other trait.
        EnumTrait(EnumTrait&& other) noexcept;

        /// @brief Converts an enumeration value to its string representation.
        /// @param value Enumeration value to be converted.
        /// @return String representation.
        template <typename T>
        static const std::string& toString(const T& value)
        {
            return reflect<T>().template get<EnumTrait>().variant(&value).name();
        }

        /// @brief Converts a string to a value of a specified type.
        /// @param[in,out] value Variable to store the converted value.
        /// @param str String to be converted to the specified value type.
        /// @return Whether the conversion was successful, false otherwise.
        template <typename T>
        static bool fromString(T& value, const std::string& str)
        {
            const auto& trait = reflect<T>().template get<EnumTrait>();
            if (!trait.contains(str))
            {
                return false;
            }

            trait.at(str).set(&value);
            return true;
        }

        /// @brief Adds a variant to the type.
        /// @param name Variant name.
        /// @param tester Function to test if a value matches the variant.
        /// @param setter Function to set the value of the variant.
        void addVariant(std::string name, Tester tester, Setter setter);

        /// @brief Adds a variant to the type.
        /// @param name Variant name.
        /// @param tester Function to test if a value matches the variant.
        /// @param setter Function to set the value of the variant.
        /// @return Trait.
        EnumTrait&& withVariant(std::string name, Tester tester, Setter setter) &&;

        /// @brief Adds a variant to the type.
        /// @param name Variant name.
        /// @tparam V Enumeration value.
        /// @return Trait.
        template <auto V>
        EnumTrait&& withVariant(std::string name) &&
        {
            return std::move(*this).withVariant(
                std::move(name), [](const void* value) { return *static_cast<const decltype(V)*>(value) == V; },
                [](void* value) { *static_cast<decltype(V)*>(value) = V; });
        }

        /// @brief Checks if a variant with the given name exists.
        /// @param name Variant name.
        /// @return Whether the variant exists.
        bool contains(const std::string& name) const;

        /// @brief Gets the variant with the specified name.
        /// @note Aborts if the variant does not exist.
        /// @param name Variant name.
        /// @return Reference.
        const Variant& at(const std::string& name) const;

        /// @brief Gets the variant associated with the given value.
        /// @param value Pointer to the value for which to retrieve the variant.
        /// @return Reference.
        const Variant& variant(const void* value) const;

        /// @brief Gets an iterator to the first variant of the type.
        /// @return Iterator.
        Iterator begin() const;

        /// @brief Gets an iterator to the last variant of the type.
        /// @return Iterator.
        static Iterator end();

        /// @brief Returns how many enum variants there are in the trait.
        /// @return Variant count.
        std::size_t size() const;

    private:
        Variant* mFirstVariant{nullptr};
        Variant* mLastVariant{nullptr};
    };

    class EnumTrait::Variant
    {
    public:
        /// @brief Constructs a variant with a given name, tester function, and setter function.
        /// @param name Variant name.
        /// @param tester Function to test if a value matches the variant.
        /// @param setter Function to set the value of the variant.
        Variant(std::string name, Tester tester, Setter setter);

        /// @brief Tests if the given value matches the variant.
        /// @param value Pointer to value.
        /// @return Whether the value matches the variant.
        bool test(const void* value) const;

        /// @brief Sets the value of the variant.
        /// @param value Pointer to value.
        void set(void* value) const;

        /// @brief Gets the name of the variant.
        /// @return Variant name.
        const std::string& name() const;

        /// @brief Returns the next variant in the linked list.
        /// @return Pointer to next variant or null if this is the last variant.
        const Variant* next() const;

    private:
        friend EnumTrait;

        std::string mName;
        Variant* mNext{nullptr};

        Tester mTester;
        Setter mSetter;
    };

    class EnumTrait::Iterator final
    {
    public:
        /// @brief Constructs.
        /// @param variant Variant.
        Iterator(const Variant* variant);

        /// @brief Copy constructs.
        /// @param iterator Iterator.
        Iterator(const Iterator& iterator) = default;

        /// @brief Copy assigns.
        /// @param iterator Iterator.
        /// @return This.
        Iterator& operator=(const Iterator&) = default;

        /// @brief Compares with another iterator.
        /// @param iterator Iterator.
        /// @return Whether both point to the same variant.
        bool operator==(const Iterator&) const = default;

        /// @brief Accesses the variant referenced by this iterator.
        /// @note Aborts if there's no variant.
        /// @return Reference to the variant.
        const Variant& operator*() const;

        /// @brief Accesses the variant referenced by this iterator.
        /// @note Aborts if there's no variant.
        /// @return Pointer to the variant.
        const Variant* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if there's no variant.
        /// @return Reference to this.
        Iterator& operator++();

    private:
        const Variant* mVariant;
    };
} // namespace cubos::core::reflection
