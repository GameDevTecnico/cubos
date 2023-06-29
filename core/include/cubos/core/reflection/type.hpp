/// @file
/// @brief Defines the Type abstract class.

#pragma once

#include <string>
#include <type_traits>

#include <cubos/core/log.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::reflection
{
    /// @brief Abstract class for classes which contains the reflection data for types.
    class Type
    {
    public:
        /// @brief Function pointer type for constructing a default instance of the type at the
        /// given location.
        using DefaultConstructor = void (*)(void*);

        /// @brief Function pointer type for constructing an instance of the type at the given
        /// location using the given instance as a source.
        using MoveConstructor = void (*)(void*, void*);

        /// @brief Function pointer type for destructing an instance of the type at the given
        /// location.
        using Destructor = void (*)(void*);

        /// @brief Base class for builders of Type objects.
        class Builder
        {
        public:
            /// @brief Construct a new Builder object.
            /// @param type Type to build.
            Builder(Type& type);

            /// @brief Sets the memory layout of the type. By default, the size and alignment are
            /// both set to 0.
            /// @param size Size of the type in bytes.
            /// @param alignment Alignment of the type in bytes.
            /// @return This builder.
            Builder& layout(std::size_t size, std::size_t alignment);

            /// @brief Sets the default constructor function for the type. Must only be set if the
            /// type's alignment is greater than 0.
            /// @param func Default constructor function.
            /// @return This builder.
            Builder& defaultConstructor(DefaultConstructor func);

            /// @brief Sets the move constructor function for the type. Must only be set if the
            /// type's alignment is greater than 0.
            /// @param func Move constructor function.
            /// @return This builder.
            Builder& moveConstructor(MoveConstructor func);

            /// @brief Sets the destructor function for the type. Must only be set if the type's
            /// alignment is greater than 0.
            /// @param func Destructor function.
            /// @return This builder.
            Builder& destructor(Destructor func);

            /// @brief Sets the layout, default constructor, move constructor and destructor
            /// functions for the type to the ones of the given type.
            /// @tparam T Type to get the default constructor and destructor from.
            /// @return This builder.
            template <typename T>
            Builder& typed()
            {
                this->layout(sizeof(T), alignof(T));

                if constexpr (std::is_default_constructible_v<T>)
                {
                    this->defaultConstructor([](void* ptr) { new (ptr) T(); });
                }

                if constexpr (std::is_move_constructible_v<T>)
                {
                    this->moveConstructor([](void* ptr, void* src) { new (ptr) T(std::move(*static_cast<T*>(src))); });
                }

                if constexpr (std::is_destructible_v<T>)
                {
                    this->destructor([](void* ptr) { static_cast<T*>(ptr)->~T(); });
                }

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

        /// @brief Get the size of the type in bytes.
        /// @return Size of the type in bytes.
        std::size_t size() const;

        /// @brief Get the alignment of the type in bytes.
        /// @return Alignment of the type in bytes.
        std::size_t alignment() const;

        /// @brief Construct a new default instance of this type on the given location. Aborts if
        /// the default constructor is not set. If the location does not have the correct alignment
        /// or size, the behavior is undefined.
        /// @param ptr Pointer to the location to construct the instance at.
        void defaultConstruct(void* ptr) const;

        /// @brief Construct a new instance of this type on the given location using the given
        /// instance as a source. Aborts if the move constructor is not set. If the location does
        /// not have the correct alignment or size, the behavior is undefined.
        /// @param ptr Pointer to the location to construct the instance at.
        /// @param src Pointer to the source instance.
        void moveConstruct(void* ptr, void* src) const;

        /// @brief Destroys an instance of this type. Aborts if the destructor is not set.
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
        std::size_t mSize;
        std::size_t mAlignment;
        DefaultConstructor mDefaultConstructor;
        MoveConstructor mMoveConstructor;
        Destructor mDestructor;
    };
} // namespace cubos::core::reflection
