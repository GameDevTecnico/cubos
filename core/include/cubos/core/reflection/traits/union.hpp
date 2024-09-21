/// @file
/// @brief Class @ref cubos::core::reflection::UnionTrait.
/// @ingroup core-reflection

#pragma once

#include <cstdint>
#include <string>

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/type.hpp>

namespace cubos::core::reflection
{
    /// @brief Provides tagged union functionality to an union type.
    /// @see See @ref examples-core-reflection-traits-union for an example of using this trait.
    /// @ingroup core-reflection
    class CUBOS_CORE_API UnionTrait
    {
    public:
        CUBOS_REFLECT;

        /// @brief Represents an individual variant within the union type.
        class Variant;

        /// @brief Used to iterate over the variants on an union trait.
        class Iterator;

        /// @brief Function pointer to test if a value matches the variant.
        using Tester = bool (*)(const void* instance);

        /// @brief Function pointer to set the value of the variant.
        using Setter = void (*)(void* instance);

        /// @brief Function pointer to get the address of the value of the variant.
        using Getter = uintptr_t (*)(const void* instance);

        ~UnionTrait();

        /// @brief Constructs.
        UnionTrait() = default;

        /// @brief Move constructs.
        /// @param other Other trait.
        UnionTrait(UnionTrait&& other) noexcept;

        /// @brief Adds a variant to the type.
        /// @param name Variant name.
        /// @param type Variant type.
        /// @param tester Function to test if a value matches the variant.
        /// @param setter Function to set the value of the variant.
        /// @param getter Function to get the address of the value of the variant.
        void addVariant(std::string name, const Type& type, Tester tester, Setter setter, Getter getter);

        /// @brief Adds a variant to the type.
        /// @param name Variant name.
        /// @param type Variant type.
        /// @param tester Function to test if a value matches the variant.
        /// @param setter Function to set the value of the variant.
        /// @param getter Function to get the address of the value of the variant.
        /// @return Trait.
        UnionTrait&& withVariant(std::string name, const Type& type, Tester tester, Setter setter, Getter getter) &&;

        /// @brief Adds a variant to the type.
        /// @param name Variant name.
        /// @param tester Function to test if a value matches the variant.
        /// @param setter Function to set the value of the variant.
        /// @param getter Function to get the address of the value of the variant.
        /// @tparam T Variant type.
        /// @return Trait.
        template <typename T>
        UnionTrait&& withVariant(std::string name, Tester tester, Setter setter, Getter getter) &&
        {
            return std::move(*this).withVariant(name, reflect<T>(), tester, setter, getter);
        }

        /// @brief Adds a variant to the type.
        /// @param tester Function to test if a value matches the variant.
        /// @param setter Function to set the value of the variant.
        /// @param getter Function to get the address of the value of the variant.
        /// @tparam T Variant type.
        /// @return Trait.
        template <typename T>
        UnionTrait&& withVariant(Tester tester, Setter setter, Getter getter) &&
        {
            return std::move(*this).withVariant(reflect<T>().name(), reflect<T>(), tester, setter, getter);
        }

        /// @brief Checks if a variant with the given name exists.
        /// @param name Variant name.
        /// @return Whether the variant exists.
        bool contains(const std::string& name) const;

        /// @brief Checks if a variant with the specified type exists.
        /// @param type Variant type.
        /// @return Whether the variant exists.
        bool contains(const Type& type) const;

        /// @brief Checks if a variant with the specified type exists.
        /// @tparam T Variant type.
        /// @return Whether the variant exists.
        template <typename T>
        bool contains() const
        {
            return this->contains(reflect<T>());
        }

        /// @brief Gets the variant with the specified name.
        /// @note Aborts if the variant does not exist.
        /// @param name Variant name.
        /// @return Reference.
        const Variant& at(const std::string& name) const;

        /// @brief Gets the first variant with the specified type.
        /// @note Aborts if the variant does not exist.
        /// @param type Variant type.
        /// @return Reference.
        const Variant& at(const Type& type) const;

        /// @brief Gets the first variant with the specified type.
        /// @note Aborts if the variant does not exist.
        /// @tparam T Variant type.
        /// @return Reference.
        template <typename T>
        const Variant& at() const
        {
            return this->at(reflect<T>());
        }

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

    class CUBOS_CORE_API UnionTrait::Variant
    {
    public:
        /// @brief Constructs a variant with a given name, type, tester function, setter function and getter function.
        /// @param name Variant name.
        /// @param type Variant type
        /// @param tester Function to test if a value matches the variant.
        /// @param setter Function to set the value of the variant.
        /// @param getter Function to get the address of the value of the variant.
        Variant(std::string name, const Type& type, Tester tester, Setter setter, Getter getter);

        /// @brief Tests if the given value matches the variant.
        /// @param value Pointer to value.
        /// @return Whether the value matches the variant.
        bool test(const void* value) const;

        /// @brief Sets the value of the variant.
        /// @param value Pointer to value.
        void set(void* value) const;

        /// @brief Gets the address of the value of the variant.
        /// @param value Pointer to value.
        /// @return Address of the value.
        const void* get(const void* value) const;

        /// @brief Gets the address of the value of the variant.
        /// @param value Pointer to value.
        /// @return Address of the value.
        void* get(void* value) const;

        /// @brief Gets the name of the variant.
        /// @return Variant name.
        const std::string& name() const;

        /// @brief Gets the type of the variant.
        /// @return Variant type.
        const Type& type() const;

        /// @brief Returns the next variant in the linked list.
        /// @return Pointer to next variant or null if this is the last variant.
        const Variant* next() const;

    private:
        friend UnionTrait;

        std::string mName;
        const Type& mType;
        Variant* mNext{nullptr};

        Tester mTester;
        Setter mSetter;
        Getter mGetter;
    };

    class CUBOS_CORE_API UnionTrait::Iterator final
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
