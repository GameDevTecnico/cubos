/// @file
/// @brief Defines the Type abstract class.

#pragma once

#include <string>

#include <cubos/core/log.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::reflection
{
    /// @brief Abstract class for classes which contains the reflection data for types.
    class Type
    {
    public:
        using DefaultConstructor = void* (*)();
        using Destructor = void (*)(void*);

        /// @brief Base class for builders of Type objects.
        class Builder
        {
        public:
            /// @brief Construct a new Builder object.
            /// @param type Type to build.
            Builder(Type& type);

            /// @brief Sets the default constructor function for the type.
            /// @param func Default constructor function.
            /// @return This builder.
            Builder& defaultConstructor(DefaultConstructor func);

            /// @brief Sets the destructor function for the type.
            /// @param func Destructor function.
            /// @return This builder.
            Builder& destructor(Destructor func);

            /// @brief Sets the default constructor and destructor functions for the type
            /// automatically given the type's default constructor and destructor.
            /// @tparam T Type to get the default constructor and destructor from.
            /// @return This builder.
            template <typename T>
            Builder& defaultConstructible()
            {
                this->defaultConstructor([]() -> void* { return new T(); });
                this->destructor([](void* ptr) { delete static_cast<T*>(ptr); });
                return *this;
            }

            /// @brief Gets a pointer to the built type. Should be freed with `delete`.
            /// @return Pointer to the built type.
            Type* get();

        private:
            Type& mType;
        };

        /// @brief Construct a new Type object.
        /// @param name Name of the type.
        Type(std::string name);

        /// @name Allow moving Type objects.
        /// @{
        Type(Type&&) = default;
        Type& operator=(Type&&) = default;
        /// @}

        /// @name Forbid copying Type objects.
        /// @{
        Type(const Type&) = delete;
        Type& operator=(const Type&) = delete;
        /// @}

        /// @brief Gets the default constructor function for the type, or `nullptr` if it is not
        /// set.
        /// @return Constructor function.
        DefaultConstructor defaultConstructor() const;

        /// @brief Gets the destructor function for the type, or `nullptr` if it is not set.
        /// @return Destructor function.
        Destructor destructor() const;

        /// @brief Get the name of the type.
        /// @return Name of the type.
        const std::string& name() const;

        /// @brief Get the short version of the name of the type - removes any template parameters.
        /// @return Short name of the type.
        const std::string& shortName() const;

        /// @brief Construct a new default instance of this type. Aborts if the default constructor
        /// is not set. The returned value must be destroyed by the destructor of this type.
        /// @return Pointer to the newly constructed instance.
        void* defaultConstruct() const;

        /// @brief Destroys an instance of this type. Aborts if the destructor is not set. The given
        /// value must have been returned by the constructor.
        /// @param instance Pointer to the instance to destroy.
        void destroy(void* instance) const;

        /// @brief Checks if this type is reflecting the given type.
        /// @tparam T
        /// @return `true` if this type is reflecting the given type, `false` otherwise.
        template <typename T>
        bool is() const
        {
            return &reflect<T>() == this;
        }

        /// @brief Checks if this type is of the given kind (PrimitiveType, ObjectType, etc.).
        /// @tparam T Kind to check.
        /// @return `true` if this type is of the given kind, `false` otherwise.
        template <typename T>
        bool isKind() const
        {
            return dynamic_cast<const T*>(this) != nullptr;
        }

        /// @brief Returns this type as the given kind (PrimitiveType, ObjectType, etc.). Aborts if
        /// this type is not of the given kind.
        /// @tparam T Kind to cast to.
        /// @return This type as the given kind.
        template <typename T>
        const T& asKind() const
        {
            auto ptr = dynamic_cast<const T*>(this);
            CUBOS_ASSERT(ptr != nullptr, "Type is not of the given kind");
            return *ptr;
        }

        /// @brief Visits the type with the given visitor.
        /// @param visitor Visitor to visit the type with.
        virtual void accept(class TypeVisitor& visitor) const = 0;

    protected:
        std::string mName;
        std::string mShortName;
        DefaultConstructor mDefaultConstructor;
        Destructor mDestructor;
    };
} // namespace cubos::core::reflection
