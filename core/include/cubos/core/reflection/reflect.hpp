/// @file
/// @brief Function @ref cubos::core::reflection::reflect and related macros.
///
/// Meant to be as minimal as possible in order to keep compile times low.
///
/// @ingroup core-reflection

#pragma once

namespace cubos::core::reflection
{
    class Type;

    /// @brief Defines the reflection function for the given type @p T.
    ///
    /// By default, this function calls the static member function `reflect` of the type, which
    /// should return a pointer to an instance of @ref Type.
    ///
    /// To implement reflection for your type, you should either:
    /// - Define a static member function `reflect` with return type `const Type&` on your type.
    /// - Specialize this struct for your type.
    ///
    /// The first option is preferred, as it is less verbose. However, when handling external
    /// types, to which member functions cannot be added, the second option is necessary.
    ///
    /// Both options can and should be shortened by using the macros:
    /// - @ref CUBOS_REFLECT - declaring the reflection member function.
    /// - @ref CUBOS_REFLECT_IMPL - defining the reflection member function.
    /// - @ref CUBOS_REFLECT_EXTERNAL_DECL - declaring the reflection specialization.
    /// - @ref CUBOS_REFLECT_EXTERNAL_IMPL - defining the reflection specialization's function.
    ///
    /// @tparam T %Type to reflect.
    /// @ingroup core-reflection
    template <typename T>
    struct Reflect
    {
        // If you get a compiler error here, either you:
        // 1. Are trying to reflect an external type and have not included the file which declares
        //    the type to be reflectable (take a look at the external/ subdir), or
        // 2. Have not implemented reflection for the external type.
        // 3. Have not implemented reflection for your type.
        //
        // Refer to the documentation above for more information on how to implement reflection on
        // your type or an external type.
        static const Type& type()
        {
            return T::reflect(); // Read the comment above if you get a compiler error here!
        }
    };

    /// @brief Reflects the given type @p T.
    ///
    /// Fails to compile if the type does not implement reflection, or, in the case of external
    /// types, if its reflection specialization has not been included.
    ///
    /// Internally, this function stores a static instance of the reflection data for the given
    /// type. Thus the reflection data is only initialized once, the first time this function is
    /// called, which means that the returned reference can be safely used as an identifier for the
    /// type.
    ///
    /// The reflection data is obtained by calling the @ref Reflect<T>::type() function.
    ///
    /// @tparam T %Type to reflect.
    /// @return %Type information.
    /// @ingroup core-reflection
    template <typename T>
    const Type& reflect()
    {
        static const Type& type = Reflect<T>::type();
        return type;
    }

    /// @cond See #765
    /// @brief Checks whether the given type @p T is reflectable.
    /// @tparam T %Type to check.
    template <typename T>
    concept Reflectable = requires
    {
        reflect<T>();
    };
    /// @endcond
} // namespace cubos::core::reflection

/// @brief Helper macro used to pass arguments with commas to other macros, wrapped in parentheses.
///
/// @code{.cpp}
/// #define FOO(T) T foo;
/// FOO(int); // expands to int foo;
/// FOO(std::map<int, int>); // error: too many arguments to macro 'FOO'
///
/// // Instead, use CUBOS_PACK:
/// #define FOO(T) CUBOS_PACK T foo;
/// FOO((int)); // expands to CUBOS_PACK(int) foo, which expands to int foo;
/// FOO((std::map<int, int>)); // expands to CUBOS_PACK(std::map<int, int>) foo, which expands to
///                            // std::map<int, int> foo;
/// @endcode
///
/// @ingroup core-reflection
#define CUBOS_PACK(...) __VA_ARGS__

/// @brief Declares a reflection method.
///
/// @code{.cpp}
/// // my_type.hpp
/// #include <cubos/core/reflection/reflect.hpp>
///
/// struct MyType
/// {
///     CUBOS_REFLECT; // declares `static const Type& reflect()`
/// };
/// @endcode
///
/// @see Meant to be used with @ref CUBOS_REFLECT_IMPL.
/// @ingroup core-reflection
#define CUBOS_REFLECT static const cubos::core::reflection::Type& reflect()

/// @brief Defines a reflection method.
///
/// @code{.cpp}
/// // my_type.cpp
/// #include "my_type.hpp"
///
/// CUBOS_REFLECT_IMPL(MyType) // defines `const Type& MyType::reflect()`
/// {
///     return /* create your type here */;
/// }
/// @endcode
///
/// @see Meant to be used with @ref CUBOS_REFLECT.
/// @param T Type to reflect.
/// @ingroup core-reflection
#define CUBOS_REFLECT_IMPL(T) const cubos::core::reflection::Type& T::reflect()

/// @brief Declares a specialization of @ref cubos::core::reflection::Reflect for a templated type.
/// @note Should be preceded by a template declaration.
///
/// @code{.cpp}
/// // templated_type_reflection.hpp
/// #include <cubos/core/reflection/reflect.hpp>
///
/// template <typename T>
/// CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE(TemplatedType<T>);
/// @endcode
///
/// @see Meant to be used with @ref CUBOS_REFLECT_EXTERNAL_IMPL.
/// @param T Type to reflect.
/// @ingroup core-reflection
#define CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE(T)                                                                        \
    struct cubos::core::reflection::Reflect<T>                                                                         \
    {                                                                                                                  \
        static const cubos::core::reflection::Type& type();                                                            \
    }

/// @brief Declares a specialization of @ref cubos::core::reflection::Reflect for a type.
///
/// @code{.cpp}
/// // not_my_type_reflection.hpp
/// #include <cubos/core/reflection/reflect.hpp>
///
/// CUBOS_REFLECT_EXTERNAL_DECL(NotMyType);
/// @endcode
///
/// @see Meant to be used with @ref CUBOS_REFLECT_EXTERNAL_IMPL.
/// @param T Type to reflect.
/// @ingroup core-reflection
#define CUBOS_REFLECT_EXTERNAL_DECL(T)                                                                                 \
    template <>                                                                                                        \
    CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE(T)

/// @brief Implements a specialization of @ref cubos::core::reflection::Reflect for a type.
///
/// @code{.cpp}
/// // not_my_type_reflection.cpp
/// #include "not_my_type_reflection.hpp"
///
/// CUBOS_REFLECT_EXTERNAL_IMPL(NotMyType)
/// {
///     return /* create your type here */;
/// }
/// @endcode
///
/// @code{.cpp}
/// // templated_type_reflection.hpp
/// template <typename T>
/// CUBOS_REFLECT_EXTERNAL_IMPL(TemplatedType<T>)
/// {
///     return /* create your type here */;
/// }
/// @endcode
///
/// @see Meant to be used with either @ref CUBOS_REFLECT_EXTERNAL_DECL or
/// @ref CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE.
/// @param T Type to reflect.
/// @ingroup core-reflection
#define CUBOS_REFLECT_EXTERNAL_IMPL(T) const cubos::core::reflection::Type& cubos::core::reflection::Reflect<T>::type()

/// @brief Both declares and implements a specialization of @ref cubos::core::reflection::Reflect for a type.
///
/// @code{.cpp}
/// // templated_type_reflection.hpp
/// #include <cubos/core/reflection/reflect.hpp>
///
/// CUBOS_REFLECT_EXTERNAL_TEMPLATE((typename T), (TemplatedType<T>))
/// {
///     return /* create your type here */;
/// }
/// @endcode
///
/// @param args Template parameters, wrapped in parentheses.
/// @param T Type to reflect, wrapped in parentheses.
/// @ingroup core-reflection
#define CUBOS_REFLECT_EXTERNAL_TEMPLATE(args, T)                                                                       \
    template <CUBOS_PACK args>                                                                                         \
    CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE(CUBOS_PACK T);                                                                \
    template <CUBOS_PACK args>                                                                                         \
    CUBOS_REFLECT_EXTERNAL_IMPL(CUBOS_PACK T)
