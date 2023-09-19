/// @file
/// @brief Class @ref cubos::core::reflection::FieldsTrait.
/// @ingroup core-reflection

#pragma once

#include <string>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::reflection
{
    /// @brief Describes the fields of a reflected type.
    /// @see See @ref examples-core-reflection-traits-fields for an example of using this trait.
    /// @ingroup core-reflection
    class FieldsTrait
    {
    public:
        /// @brief Interface for obtaining the address of a field on a given instance.
        /// @note We could use std::function and a capturing lambda instead of this interface, but
        /// that would require including the bloated `<functional>` header here, which we want to
        /// avoid.
        class AddressOf
        {
        public:
            virtual ~AddressOf() = default;

            /// @brief Gets the address of the field on a given instance.
            /// @param instance Pointer to the instance.
            /// @return Address of the field on the given instance.
            virtual uintptr_t get(const void* instance) const = 0;
        };

        /// @brief Implementation of @ref AddressOf for a pointer to member.
        /// @tparam O Object type.
        /// @tparam F Field type.
        template <typename O, typename F>
        class AddressOfImpl : public AddressOf
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

        /// @brief Describes a field of a reflected type.
        class Field
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

            /// @brief Returns the next field in the linked list.
            /// @return Pointer to next field or null if this is the last field.
            const Field* next() const;

        private:
            friend FieldsTrait;

            const Type& mType;
            std::string mName;
            AddressOf* mAddressOf;
            Field* mNext;
        };

        ~FieldsTrait();

        /// @brief Constructs.
        FieldsTrait();

        /// @brief Move constructs.
        /// @param other Other trait.
        FieldsTrait(FieldsTrait&& other);

        /// @brief Adds a field to the type. The getter will be deleted using `delete` and thus
        /// must be allocated using `new`.
        /// @param type Field type.
        /// @param name Field name.
        /// @param addressOf Getter for the address of the field on a given instance.
        /// @return Trait.
        FieldsTrait&& withField(const Type& type, std::string name, AddressOf* addressOf) &&;

        /// @brief Sets the copy constructor of the type.
        /// @tparam O Object type.
        /// @tparam F Field type.
        /// @param field Field name.
        /// @param pointer Field pointer.
        /// @return Trait.
        template <typename O, typename F>
        FieldsTrait&& withField(const std::string& field, F O::*pointer) &&
        {
            return std::move(*this).withField(reflect<F>(), field, new AddressOfImpl<O, F>(pointer));
        }

        /// @brief Gets the field with the given type.
        /// @param name Field name.
        /// @return Field with the given name, or null if no such field exists.
        const Field* field(const std::string& name) const;

        /// @brief Gets the first field of the type.
        /// @return Pointer to the first field of the type, or null if there are no fields.
        const Field* firstField() const;

    private:
        Field* mFirstField;
        Field* mLastField;
    };
} // namespace cubos::core::reflection
