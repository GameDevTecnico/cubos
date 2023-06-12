/// @file
/// Defines the ObjectType class, which is an implementation of Type for object types.

#pragma once

#include <memory>
#include <vector>

#include <cubos/core/log.hpp>
#include <cubos/core/reflection/type.hpp>

namespace cubos::core::reflection
{
    /// @brief Implementation of Type for object types - types which can be further decomposed
    /// into fields with their own types (e.g. `struct`, `class`).
    class ObjectType : public Type
    {
    public:
        /// @brief Represents a field of an object type. Can be used to get the value of the field
        /// of an instance of the type.
        class Field
        {
        public:
            /// @brief Construct a new Field object.
            /// @param name Name of the field.
            /// @param type Type of the field.
            Field(std::string name, const Type& type);

            /// @brief Gets the name of the field.
            /// @return Name of the field.
            const std::string& name() const;

            /// @brief Gets the type of the field.
            /// @return Type of the field.
            const Type& type() const;

            /// @name Getters to the field of an instance of the type.
            /// @brief Gets a pointer or reference to the this field of the given instance of the
            /// type. Must be a valid instance of the type, otherwise, undefined behavior occurs.
            /// @param object Pointer to the instance of the type.
            /// @return Pointer or reference to the field.
            /// @{
            virtual const void* get(const void* object) const = 0;
            virtual void* get(void* object) const = 0;

            /// @tparam T Type of the field. Must be the same as the reflected type of the field.
            template <typename T>
            const T& get(const void* object) const
            {
                CUBOS_ASSERT(mType.is<T>(), "Expected field of type {} but got {}", reflect<T>().name(), mType.name());
                return *static_cast<const T*>(get(object));
            }

            /// @tparam T Type of the field. Must be the same as the reflected type of the field.
            template <typename T>
            T& get(void* object) const
            {
                CUBOS_ASSERT(mType.is<T>(), "Expected field of type {} but got {}", reflect<T>().name(), mType.name());
                return *static_cast<T*>(get(object));
            }
            /// @}

        private:
            std::string mName;
            const Type& mType;
        };

    private:
        /// @brief Implementation of Field for fields which are pointers to members of the class
        /// of instances of the type.
        /// @tparam T Type of the field.
        /// @tparam C Type of the class which contains the field.
        template <typename T, typename C>
        class FieldPointerToMember : public Field
        {
        public:
            /// @brief Construct a new FieldPointerToMember object.
            /// @param name Name of the field.
            /// @param type Type of the field.
            /// @param field Pointer to the field.
            FieldPointerToMember(std::string name, const Type& type, T C::*field)
                : Field(std::move(name), type)
                , mField(field)
            {
            }

            virtual const void* get(const void* object) const override
            {
                return &(static_cast<const C*>(object)->*mField);
            }

            virtual void* get(void* object) const override
            {
                return &(static_cast<C*>(object)->*mField);
            }

        private:
            T C::*mField;
        };

    public:
        /// @brief Builder for creating ObjectType's.
        class Builder final : public Type::Builder
        {
        public:
            /// @brief Construct a new Builder object.
            /// @param type Type to build.
            Builder(ObjectType& type);

            /// @brief Adds a field to the type and returns the type. The field must be a pointer to
            /// a member of the class of instances of the type, otherwise, undefined behavior occurs.
            /// @tparam T Type of the field.
            /// @tparam C Type of the class which contains the field.
            /// @param name Name of the field.
            /// @param field Pointer to the field.
            /// @return Type to which the field was added.
            template <typename T, typename C>
            Builder& field(std::string name, T C::*field)
            {
                mType.mFields.emplace_back(
                    std::make_unique<FieldPointerToMember<T, C>>(std::move(name), reflect<T>(), field));
                return *this;
            }

        private:
            ObjectType& mType;
        };

        /// @brief Creates a new Builder for an ObjectType with the given name.
        /// @param name Name of the type.
        /// @return Builder for the type.
        static Builder build(std::string name);

        /// @brief Gets the fields of the type.
        /// @return Fields of the type.
        const std::vector<std::unique_ptr<const Field>>& fields() const;

        /// @brief Gets the field of the type with the given name.
        /// @param name Name of the field.
        /// @return Field of the type with the given name.
        const Field& field(std::string_view name) const;

        virtual void accept(class TypeVisitor& visitor) const override;

    private:
        ObjectType(std::string name);

        std::vector<std::unique_ptr<const Field>> mFields;
    };
} // namespace cubos::core::reflection
