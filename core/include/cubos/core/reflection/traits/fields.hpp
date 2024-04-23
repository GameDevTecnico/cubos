/// @file
/// @brief Class @ref cubos::core::reflection::FieldsTrait.
/// @ingroup core-reflection

#pragma once

#include <cstdint>
#include <string>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::reflection
{
    /// @brief Describes the fields of a reflected type.
    /// @see See @ref examples-core-reflection-traits-fields for an example of using this trait.
    /// @ingroup core-reflection
    class CUBOS_CORE_API FieldsTrait
    {
    public:
        CUBOS_REFLECT;

        /// @brief Interface for obtaining the address of a field on a given instance.
        /// @note We could use std::function and a capturing lambda instead of this interface, but
        /// that would require including the bloated `<functional>` header here, which we want to
        /// avoid.
        class AddressOf;

        /// @brief Implementation of @ref AddressOf for a pointer to member.
        /// @tparam O Object type.
        /// @tparam F Field type.
        template <typename O, typename F>
        class AddressOfImpl;

        /// @brief Describes a field of a reflected type.
        class Field;

        /// @brief Used to iterate over the fields on a fields trait.
        class Iterator;

        /// @brief Provides mutable access to an object.
        class View;

        /// @brief Provides immutable access to an object.
        class ConstView;

        ~FieldsTrait();

        /// @brief Constructs.
        FieldsTrait() = default;

        /// @brief Move constructs.
        /// @param other Other trait.
        FieldsTrait(FieldsTrait&& other) noexcept;

        /// @brief Adds a field to the type. The getter will be deleted using `delete` and thus
        /// must be allocated using `new`.
        /// @param type Field type.
        /// @param name Field name.
        /// @param addressOf Getter for the address of the field on a given instance.
        void addField(const Type& type, std::string name, AddressOf* addressOf);

        /// @copydoc addField(const Type&, std::string, AddressOf*)
        /// @return Trait.
        FieldsTrait&& withField(const Type& type, std::string name, AddressOf* addressOf) &&;

        /// @brief Adds a field to the type.
        /// @tparam O Object type.
        /// @tparam F Field type.
        /// @param name Field name.
        /// @param pointer Field pointer.
        template <typename O, typename F>
        void addField(std::string name, F O::*pointer)
        {
            this->addField(reflect<F>(), std::move(name), new AddressOfImpl<O, F>(pointer));
        }

        /// @copydoc addField(std::string, F O::*)
        /// @return Trait.
        template <typename O, typename F>
        FieldsTrait&& withField(std::string name, F O::*pointer) &&
        {
            this->addField(std::move(name), pointer);
            return std::move(*this);
        }

        /// @brief Gets the field with the given name.
        /// @param name Field name.
        /// @return Field with the given name, or null if no such field exists.
        const Field* field(const std::string& name) const;

        /// @brief Gets an iterator to the first field of the type.
        /// @return Iterator.
        Iterator begin() const;

        /// @brief Gets an iterator which represents the end of the field list of a type.
        /// @return Iterator.
        static Iterator end();

        /// @brief Returns how many fields there are in the trait.
        /// @return Field count.
        std::size_t size() const;

        /// @brief Returns a view of the given object instance.
        /// @param instance Object instance.
        /// @return Object view.
        View view(void* instance) const;

        /// @copydoc view(void*) const
        ConstView view(const void* instance) const;

    private:
        Field* mFirstField{nullptr};
        Field* mLastField{nullptr};
    };

    class CUBOS_CORE_API FieldsTrait::AddressOf
    {
    public:
        virtual ~AddressOf() = default;

        /// @brief Gets the address of the field on a given instance.
        /// @param instance Pointer to the instance.
        /// @return Address of the field on the given instance.
        virtual uintptr_t get(const void* instance) const = 0;
    };

    template <typename O, typename F>
    class FieldsTrait::AddressOfImpl final : public AddressOf
    {
    public:
        /// @brief Constructs.
        /// @param pointer Pointer to member.
        AddressOfImpl(F O::*pointer)
            : mPointer(pointer)
        {
        }

        uintptr_t get(const void* instance) const override
        {
            return reinterpret_cast<uintptr_t>(&(static_cast<const O*>(instance)->*mPointer));
        }

    private:
        F O::*mPointer;
    };

    class CUBOS_CORE_API FieldsTrait::Field final
    {
    public:
        ~Field();

        /// @brief Constructs using the given address of getter. Will be deleted using
        /// `delete` and thus must be allocated using `new`.
        /// @param type Field type.
        /// @param name Field name.
        /// @param addressOf Getter for the address of the field on a given instance.
        Field(const Type& type, std::string name, AddressOf* addressOf);

        /// @brief Returns the type of the field.
        /// @return Type of the field.
        const Type& type() const;

        /// @brief Returns the name of the field.
        /// @return Name of the field.
        const std::string& name() const;

        /// @brief Returns the next field in the linked list.
        /// @return Pointer to next field or null if this is the last field.
        const Field* next() const;

    private:
        friend FieldsTrait;
        friend View;
        friend ConstView;

        /// @brief Returns the address of the field on a given instance.
        /// @param instance Pointer to the instance.
        /// @return Address of the field on the given instance.
        uintptr_t addressOf(const void* instance) const;

        /// @brief Returns a pointer to the field on a given instance.
        /// @param instance Pointer to the instance.
        /// @return Pointer to the field on the given instance.
        void* pointerTo(void* instance) const;

        /// @copydoc pointerTo(void*)
        const void* pointerTo(const void* instance) const;

        const Type& mType;
        std::string mName;
        AddressOf* mAddressOf;
        Field* mNext{nullptr};
    };

    class CUBOS_CORE_API FieldsTrait::Iterator final
    {
    public:
        /// @brief Constructs.
        /// @param field Field.
        Iterator(const Field* field);

        /// @brief Copy constructs.
        /// @param iterator Iterator.
        Iterator(const Iterator& iterator) = default;

        Iterator& operator=(const Iterator&) = default;
        bool operator==(const Iterator&) const = default;
        bool operator!=(const Iterator&) const = default;

        /// @brief Accesses the field referenced by this iterator.
        /// @note Aborts if there's no field.
        /// @return Reference to the field.
        const Field& operator*() const;

        /// @brief Accesses the field referenced by this iterator.
        /// @note Aborts if there's no field.
        /// @return Pointer to the field.
        const Field* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if there's no field.
        /// @return Reference to this.
        Iterator& operator++();

    private:
        const Field* mField;
    };

    class CUBOS_CORE_API FieldsTrait::View
    {
    public:
        /// @brief Used to iterate over the fields of an object.
        class Iterator;

        /// @brief Constructs.
        /// @param trait Trait.
        /// @param instance Instance.
        View(const FieldsTrait& trait, void* instance);

        /// @brief Gets a pointer to the value of the given field on the object.
        /// @param field Field.
        /// @return Field value.
        void* get(const Field& field) const;

        /// @brief Gets a pointer to the value of the field with the given name on the object.
        /// @note Returns null if field doesn't exist.
        /// @param name Field name.
        /// @return Field value.
        void* get(const std::string& name) const;

        /// @brief Gets an iterator to the first field.
        /// @return Iterator.
        Iterator begin() const;

        /// @brief Gets an iterator to the field after the last field.
        /// @return Iterator.
        Iterator end() const;

    private:
        const FieldsTrait& mTrait;
        void* mInstance;
    };

    class CUBOS_CORE_API FieldsTrait::ConstView
    {
    public:
        /// @brief Used to iterate over the fields of an object.
        class Iterator;

        /// @brief Constructs.
        /// @param trait Trait.
        /// @param instance Instance.
        ConstView(const FieldsTrait& trait, const void* instance);

        /// @brief Gets a pointer to the value of the given field on the object.
        /// @param field Field.
        /// @return Field value.
        const void* get(const Field& field) const;

        /// @brief Gets a pointer to the value of the field with the given name on the object.
        /// @note Returns null if field doesn't exist.
        /// @param name Field name.
        /// @return Field value.
        const void* get(const std::string& name) const;

        /// @brief Gets an iterator to the first field.
        /// @return Iterator.
        Iterator begin() const;

        /// @brief Gets an iterator to the field after the last field.
        /// @return Iterator.
        Iterator end() const;

    private:
        const FieldsTrait& mTrait;
        const void* mInstance;
    };

    class CUBOS_CORE_API FieldsTrait::View::Iterator
    {
    public:
        /// @brief Output structure for the iterator.
        struct Output
        {
            const Field* field; ///< Field.
            void* value;        ///< Value.
        };

        /// @brief Constructs.
        /// @param view View.
        /// @param field Field.
        Iterator(const View& view, const Field* field);

        /// @brief Copy constructs.
        /// @param other Other iterator.
        Iterator(const Iterator& other) = default;

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to the same field.
        bool operator==(const Iterator& other) const;

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to different fields.
        bool operator!=(const Iterator& /*other*/) const;

        /// @brief Accesses the field referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Output.
        const Output& operator*() const;

        /// @brief Accesses the field referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Output.
        const Output* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if out of bounds.
        /// @return Reference to this.
        Iterator& operator++();

    private:
        void* mInstance;
        const Field* mField;
        mutable Output mOutput;
    };

    class CUBOS_CORE_API FieldsTrait::ConstView::Iterator
    {
    public:
        /// @brief Output structure for the iterator.
        struct Output
        {
            const Field* field; ///< Field.
            const void* value;  ///< Value.
        };

        /// @brief Constructs.
        /// @param view View.
        /// @param field Field.
        Iterator(const ConstView& view, const Field* field);

        /// @brief Copy constructs.
        /// @param other Other iterator.
        Iterator(const Iterator& other) = default;

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to the same field.
        bool operator==(const Iterator& other) const;

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to different fields.
        bool operator!=(const Iterator& /*other*/) const;

        /// @brief Accesses the field referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Output.
        const Output& operator*() const;

        /// @brief Accesses the field referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Output.
        const Output* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if out of bounds.
        /// @return Reference to this.
        Iterator& operator++();

    private:
        const void* mInstance;
        const Field* mField;
        mutable Output mOutput;
    };
} // namespace cubos::core::reflection
