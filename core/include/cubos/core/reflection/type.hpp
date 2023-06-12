/// @file
/// @brief Defines the Type abstract class, the reflect() function and reflection macros.

#pragma once

#include <string>

#include <cubos/core/log.hpp>

/// @brief Helper macro used to pass arguments with commas to other macros, wrapped in parentheses.
/// @code
/// #define FOO(T) T foo;
/// FOO(int); // expands to int foo;
/// FOO(std::map<int, int>); // error: too many arguments to macro 'FOO'
///
/// // Instead, use CUBOS_PACK:
/// #define FOO(T) CUBOS_PACK T foo;
/// FOO((int)); // expands to PACK(int) foo, which expands to int foo;
/// FOO((std::map<int, int>)); // expands to PACK(std::map<int, int>) foo, which expands to std::map<int, int> foo;
/// @endcode
#define CUBOS_PACK(...) __VA_ARGS__

/// @brief Declares a reflection method. Can either be used with the CUBOS_REFLECT_IMPL macro or
/// defined immediately after this macro.
/// @see CUBOS_REFLECT_IMPL
#define CUBOS_REFLECT static const cubos::core::reflection::Type* reflect()

/// @brief Defines a reflection method declared with CUBOS_REFLECT. Should be followed by the
/// definition of the method, which should return a Type object.
/// @param T Type to reflect.
/// @see CUBOS_REFLECT_DECL
#define CUBOS_REFLECT_IMPL(T) const cubos::core::reflection::Type* T::reflect()

/// @brief Defines a specialization of the Reflect class for a type. Should be used with the
/// CUBOS_REFLECT_EXTERNAL_IMPL macro.
/// @param args Template parameters wrapped in parentheses.
/// @param T Type to reflect.
/// @see CUBOS_REFLECT_EXTERNAL_IMPL
#define CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE(args, T)                                                                  \
    template <CUBOS_PACK args>                                                                                         \
    struct cubos::core::reflection::Reflect<T>                                                                         \
    {                                                                                                                  \
        static const cubos::core::reflection::Type* type();                                                            \
    }

/// @brief Defines a specialization of the Reflect class for a type. Should be used with the
/// CUBOS_REFLECT_EXTERNAL_IMPL macro.
/// @param T Type to reflect.
/// @see CUBOS_REFLECT_EXTERNAL_IMPL
#define CUBOS_REFLECT_EXTERNAL_DECL(T) CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE((), T)

/// @brief Implements the specialization of the Reflect class for a type declared with the
/// CUBOS_REFLECT_EXTERNAL_DECL or CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE macros.
/// @param T Type to reflect.
/// @see CUBOS_REFLECT_EXTERNAL_DECL CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE
#define CUBOS_REFLECT_EXTERNAL_IMPL(T) const cubos::core::reflection::Type* cubos::core::reflection::Reflect<T>::type()

/// @brief Defines a specialization of the Reflect class for a type with template parameters.
/// @param args Template parameters, wrapped in parentheses.
/// @param T Type to reflect, wrapped in parentheses.
#define CUBOS_REFLECT_EXTERNAL_TEMPLATE(args, T)                                                                       \
    CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE(args, CUBOS_PACK T);                                                          \
    template <CUBOS_PACK args>                                                                                         \
    CUBOS_REFLECT_EXTERNAL_IMPL(CUBOS_PACK T)

namespace cubos::core::reflection
{
    /// @brief Returns the reflection data for the given type. Fails to compile if the type does
    /// not implement reflection.
    /// @details Internally, this function stores a static instance of the reflection data for the
    /// given type. This means that the reflection data is only initialized once. This data is
    /// initialized by calling Reflect<T>::type().
    /// @tparam T Type to reflect.
    /// @return Reflection data for the given type.
    template <typename T>
    const class Type& reflect();

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

    /// @brief Specifies the reflection function for the given type. By default, this function is
    /// the static member function `static const Type& reflect()` of the type.
    ///
    /// @details To implement reflection for a type, you can either add a static member function
    /// `static const Type& reflect()` to the type, or specialize this struct for the type. For
    /// example, the first option would look like:
    ///
    /// @code
    /// // my_type.hpp
    /// struct MyType
    /// {
    ///     static const Type& reflect();
    /// };
    ///
    /// // my_type.cpp
    /// const Type& MyType::reflect()
    /// {
    ///     static auto type = PrimitiveType{"MyType"};
    ///     return type;
    /// }
    /// @endcode
    ///
    /// The second option would look like:
    ///
    /// @code
    /// // my_type.hpp
    /// struct cubos::core::reflection::Reflect<MyType>
    /// {
    ///     static const Type& type();
    /// };
    ///
    /// // my_type.cpp
    /// const Type& cubos::core::reflection::Reflect<MyType>::type()
    /// {
    ///     static auto type = PrimitiveType{"MyType"};
    ///     return type;
    /// }
    /// @endcode
    ///
    /// The first option is preferred, as it is less verbose. However, the second option is
    /// necessary if you do cannot modify the type (e.g. if it is a type from a third-party
    /// library or the standard library).
    ///
    /// The first option can be shortened by using the `CUBOS_REFLECT_DECL` and
    /// `CUBOS_REFLECT_IMPL` macros:
    ///
    /// @code
    /// // my_type.hpp
    /// struct MyType
    /// {
    ///     CUBOS_REFLECT_DECL;
    /// };
    ///
    /// // my_type.cpp
    /// CUBOS_REFLECT_IMPL(MyType, PrimitiveType{"MyType"});
    /// @endcode
    ///
    /// The second option can be shortened by using the `CUBOS_REFLECT_EXTERNAL_DECL` macro and
    /// `CUBOS_REFLECT_EXTERNAL_IMPL` macros:
    ///
    /// @code
    /// // my_type.hpp
    /// struct MyType {};
    /// CUBOS_REFLECT_EXTERNAL_DECL(MyType);
    ///
    /// // my_type.cpp
    /// CUBOS_REFLECT_EXTERNAL_IMPL(MyType, PrimitiveType{"MyType"});
    /// @endcode
    ///
    /// @tparam T Type to reflect.
    template <typename T>
    struct Reflect
    {
        // If you get a compiler error here, you need to implement reflection for the type. Either:
        // 1. Include the a file which implements reflection for the type, or
        // 2. Specialize this struct for the type, or
        // 3. Add a static member function `static Type reflect()` to the type.
        // Refer to the documentation above for more information.
        static const Type* type()
        {
            return T::reflect(); // Read the comment above if you get a compiler error here.
        }
    };

    template <typename T>
    const Type& reflect()
    {
        static const auto* type = Reflect<T>::type();
        return *type;
    }
} // namespace cubos::core::reflection
