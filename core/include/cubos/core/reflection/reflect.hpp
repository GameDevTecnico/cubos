/// @file
/// @brief Defines the reflect() function and reflection macros. Meant to be as minimal as possible
/// in order to keep compile times low.

#pragma once

namespace cubos::core::reflection
{
    class Type;

    /// @brief Specifies the reflection function for the given type. By default, this function is
    /// the static member function `static const Type* reflect()` of the type. The reflection
    /// function is responsible for creating the Type object for the type.
    ///
    /// @details To implement reflection for a type, you should use one of the following options:
    /// - Define a static member function `static const Type* reflect()` in the type.
    /// - Define a specialization of the this class for the type.
    ///
    /// The first option is preferred, as it is less verbose. However, for external types, the
    /// second option is necessary, since you cannot add members to external types.
    ///
    /// Both options can be shortened by using the CUBOS_REFLECT* macros. For example, the first
    /// option would look like:
    ///
    /// @code
    /// // my_type.hpp
    /// #include <cubos/core/reflection/reflect.hpp>
    ///
    /// struct MyType
    /// {
    ///     CUBOS_REFLECT; // declares `static const Type* reflect()`
    /// };
    ///
    /// // my_type.cpp
    /// #include "my_type.hpp"
    ///
    /// CUBOS_REFLECT_IMPL(MyType) // defines `const Type* MyType::reflect()`
    /// {
    ///     return PrimitiveType::build("MyType").get();
    /// }
    /// @code
    ///
    /// Implementing reflection for an external type would look like:
    ///
    /// @code
    /// // not_my_type_reflection.hpp
    /// #include <cubos/core/reflection/reflect.hpp>
    ///
    /// CUBOS_REFLECT_EXTERNAL_DECL(NotMyType);
    ///
    /// // not_my_type_reflection.cpp
    /// #include "not_my_type_reflection.hpp"
    ///
    /// CUBOS_REFLECT_EXTERNAL_IMPL(NotMyType)
    /// {
    ///     return PrimitiveType::build("NotMyType").get();
    /// }
    /// @endcode
    ///
    /// @tparam T Type to reflect.
    template <typename T>
    struct Reflect
    {
        // If you get a compiler error here, either you:
        // 1. Are trying to reflect an external type and have not included the file which declares
        //    the type to be reflectable, or
        // 2. Have not implemented reflection for the external type.
        // 3. Have not implemented reflection for your type.
        //
        // Refer to the documentation above for more information on how to implement reflection on
        // your type or an external type.
        static const Type* type()
        {
            return T::reflect(); // Read the comment above if you get a compiler error here!
        }
    };

    /// @brief Gets the reflection data for the given type. Fails to compile if the type does not
    /// implement reflection, or if its an external type and its reflection has not been included
    /// For external types, the file which declares the type to be reflectable must be included.
    ///
    /// @details Internally, this function stores a static instance of the reflection data for the
    /// given type. This means that the reflection data is only initialized once, the first time
    /// this function is called, and that the returned reference can be safely used as an
    /// identifier for the type.
    ///
    /// This data is initialized by calling the Reflect<T>::type() function. Refer to the
    /// documentation for the Reflect class for more information on how to implement reflection.
    ///
    /// @tparam T Type to reflect.
    /// @return Reflection data for the given type.
    /// @see Reflect
    template <typename T>
    const Type& reflect()
    {
        static const auto* type = Reflect<T>::type();
        return *type;
    }
} // namespace cubos::core::reflection

/// @brief Helper macro used to pass arguments with commas to other macros, wrapped in parentheses.
/// @details
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

/// @brief Defines a reflection method previously declared with CUBOS_REFLECT. Should be followed
/// by the definition of the method, which should return a pointer to a new Type object.
/// @param T Type to reflect.
/// @see CUBOS_REFLECT_DECL
#define CUBOS_REFLECT_IMPL(T) const cubos::core::reflection::Type* T::reflect()

/// @brief Declares a specialization of the Reflect class for a templated type. Should be preceded
/// by a template declaration and implemented with the CUBOS_REFLECT_EXTERNAL_IMPL macro.
/// @param T Type to reflect.
/// @see CUBOS_REFLECT_EXTERNAL_IMPL
#define CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE(T)                                                                        \
    struct cubos::core::reflection::Reflect<T>                                                                         \
    {                                                                                                                  \
        static const cubos::core::reflection::Type* type();                                                            \
    }

/// @brief Declares a specialization of the Reflect class for a type. Should be implemented with
/// the CUBOS_REFLECT_EXTERNAL_IMPL macro.
/// @param T Type to reflect.
/// @see CUBOS_REFLECT_EXTERNAL_IMPL
#define CUBOS_REFLECT_EXTERNAL_DECL(T)                                                                                 \
    template <>                                                                                                        \
    CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE(T)

/// @brief Implements the specialization of the Reflect class for a type declared with the
/// CUBOS_REFLECT_EXTERNAL_DECL or CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE macros.
/// @param T Type to reflect.
/// @see CUBOS_REFLECT_EXTERNAL_DECL CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE
#define CUBOS_REFLECT_EXTERNAL_IMPL(T) const cubos::core::reflection::Type* cubos::core::reflection::Reflect<T>::type()

/// @brief Inline version of CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE. Implements the specialization of
/// the Reflect class for a templated type.
/// @param args Template parameters, wrapped in parentheses.
/// @param T Type to reflect, wrapped in parentheses.
#define CUBOS_REFLECT_EXTERNAL_TEMPLATE(args, T)                                                                       \
    template <CUBOS_PACK args>                                                                                         \
    CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE(CUBOS_PACK T);                                                                \
    template <CUBOS_PACK args>                                                                                         \
    CUBOS_REFLECT_EXTERNAL_IMPL(CUBOS_PACK T)