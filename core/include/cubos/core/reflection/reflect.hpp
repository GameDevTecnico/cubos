/// @file
/// @brief Function @ref cubos::core::reflection::reflect and related macros.
///
/// Meant to be as minimal as possible in order to keep compile times low.
///
/// @ingroup core-reflection

#pragma once

#include <cstddef>

#include <cubos/core/api.hpp>

namespace cubos::core::reflection
{
    class Type;

    /// @brief Creates a new reflection type with a unique name created from a type name and a file path, and with a
    /// minimal @ref ConstructibleTrait.
    /// @param name Type name.
    /// @param file Path of the file where the type is defined.
    /// @param size Type size in bytes.
    /// @param alignment Type alignment in bytes.
    /// @param destructor Type destructor.
    /// @ingroup core-reflection
    CUBOS_CORE_API const Type& makeAnonymousType(const char* name, const char* file, std::size_t size,
                                                 std::size_t alignment, void (*destructor)(void*));

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
    /// - @ref CUBOS_REFLECT - declaring the reflection member functions.
    /// - @ref CUBOS_REFLECT_IMPL - defining the reflection member functions.
    /// - @ref CUBOS_REFLECT_EXTERNAL_DECL - declaring the reflection specialization.
    /// - @ref CUBOS_REFLECT_EXTERNAL_IMPL - defining the reflection specialization's functions.
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
        static const Type& get()
        {
            constexpr bool HasReflectionMethod = requires()
            {
                T::reflectGet();
            };
            static_assert(HasReflectionMethod, "Type does not implement reflection");
            return T::reflectGet();
        }
    };

    /// @brief Reflects the given type @p T.
    ///
    /// Fails to compile if the type does not implement reflection, or, in the case of external
    /// types, if its reflection specialization has not been included.
    ///
    /// @tparam T %Type to reflect.
    /// @return %Type information.
    /// @ingroup core-reflection
    template <typename T>
    const Type& reflect()
    {
        return Reflect<T>::get();
    }
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

/// @brief Helper macro with no effect.
#define CUBOS_EMPTY

/// @brief Declares the static reflection methods.
///
/// @code{.cpp}
/// // my_type.hpp
/// #include <cubos/core/reflection/reflect.hpp>
///
/// struct MyType
/// {
///     // declares `static const Type& reflectGet()` and `static const Type& reflectMake()`
///     CUBOS_REFLECT;
/// };
/// @endcode
///
/// @see Meant to be used with @ref CUBOS_REFLECT_IMPL.
/// @ingroup core-reflection
#define CUBOS_REFLECT                                                                                                  \
    static const cubos::core::reflection::Type& reflectGet();                                                          \
    static const cubos::core::reflection::Type& reflectMake();                                                         \
    static_assert(true, "") /* Here just to force the user to enter a semicolon */

/// @brief Defines a reflection method for a non-templated type.
///
/// @code{.cpp}
/// // my_type.cpp
/// #include "my_type.hpp"
///
/// CUBOS_REFLECT_IMPL(MyType) // defines `const Type& MyType::reflectGet()` and `const Type& MyType::reflectMake()`.
/// {
///     return /* create your type here */;
/// }
/// @endcode
///
/// @see Meant to be used with @ref CUBOS_REFLECT.
/// @param T Type to reflect.
/// @ingroup core-reflection
#define CUBOS_REFLECT_IMPL(T)                                                                                          \
    const ::cubos::core::reflection::Type& T::reflectGet()                                                             \
    {                                                                                                                  \
        static const ::cubos::core::reflection::Type& type = T::reflectMake();                                         \
        return type;                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    const ::cubos::core::reflection::Type& T::reflectMake()

/// @brief Similar to @ref CUBOS_REFLECT_IMPL but tailored to templated types.
///
/// @code{.cpp}
/// // templated_type.cpp
/// #include "templated_type.hpp"
///
/// CUBOS_REFLECT_IMPL((typename T), (TemplatedType<T>))
/// {
///     return /* create your type here */;
/// }
/// @endcode
///
/// @see Meant to be used with @ref CUBOS_REFLECT.
/// @param args Template arguments.
/// @param T Type to reflect.
/// @ingroup core-reflection
#define CUBOS_REFLECT_TEMPLATE_IMPL(args, T)                                                                           \
    template <CUBOS_PACK args>                                                                                         \
    const ::cubos::core::reflection::Type& CUBOS_PACK T::reflectGet()                                                  \
    {                                                                                                                  \
        static const ::cubos::core::reflection::Type& type = CUBOS_PACK T::reflectMake();                              \
        return type;                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    template <CUBOS_PACK args>                                                                                         \
    const ::cubos::core::reflection::Type& CUBOS_PACK T::reflectMake()

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
/// @param api API macro to use. Set to @ref CUBOS_EMPTY if not using an API macro.
/// @param T Type to reflect.
/// @ingroup core-reflection
#define CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE(api, T)                                                                   \
    struct api cubos::core::reflection::Reflect<T>                                                                     \
    {                                                                                                                  \
        static const ::cubos::core::reflection::Type& get();                                                           \
        static const ::cubos::core::reflection::Type& make();                                                          \
    }

/// @brief Declares a specialization of @ref cubos::core::reflection::Reflect for a type.
///
/// @code{.cpp}
/// // not_my_type_reflection.hpp
/// #include <cubos/core/reflection/reflect.hpp>
///
/// CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_CORE_API, NotMyType);
/// @endcode
///
/// @see Meant to be used with @ref CUBOS_REFLECT_EXTERNAL_IMPL.
/// @param api API macro to use. Set to @ref CUBOS_EMPTY if not using an API macro.
/// @param T Type to reflect.
/// @ingroup core-reflection
#define CUBOS_REFLECT_EXTERNAL_DECL(api, T)                                                                            \
    template <>                                                                                                        \
    CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE(api, T)

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
/// @see Meant to be used with either @ref CUBOS_REFLECT_EXTERNAL_DECL or
/// @ref CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE.
/// @param T Type to reflect.
/// @ingroup core-reflection
#define CUBOS_REFLECT_EXTERNAL_IMPL(T)                                                                                 \
    const ::cubos::core::reflection::Type& ::cubos::core::reflection::Reflect<T>::get()                                \
    {                                                                                                                  \
        static const ::cubos::core::reflection::Type& type = Reflect<T>::make();                                       \
        return type;                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    const ::cubos::core::reflection::Type& ::cubos::core::reflection::Reflect<T>::make()

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
    CUBOS_REFLECT_EXTERNAL_DECL_TEMPLATE(CUBOS_EMPTY, CUBOS_PACK T);                                                   \
    template <CUBOS_PACK args>                                                                                         \
    const ::cubos::core::reflection::Type& ::cubos::core::reflection::Reflect<CUBOS_PACK T>::get()                     \
    {                                                                                                                  \
        static const ::cubos::core::reflection::Type& type = Reflect<CUBOS_PACK T>::make();                            \
        return type;                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    template <CUBOS_PACK args>                                                                                         \
    const ::cubos::core::reflection::Type& ::cubos::core::reflection::Reflect<CUBOS_PACK T>::make()

/// @brief Defines minimal reflection for a type private to a compilation unit.
///
/// Equivalent to @ref CUBOS_REFLECT with an implementation which uses @ref makeAnonymousType.
///
/// @code{.cpp}
/// // my_type.cpp
/// #include <cubos/core/reflection/reflect.hpp>
///
/// namespace
/// {
///     struct State
///     {
///         CUBOS_ANONYMOUS_REFLECT(State);
///     };
/// }
/// @endcode
///
/// @ingroup core-reflection
#define CUBOS_ANONYMOUS_REFLECT(...)                                                                                   \
    static inline const cubos::core::reflection::Type& reflectGet()                                                    \
    {                                                                                                                  \
        static const ::cubos::core::reflection::Type& type = reflectMake();                                            \
        return type;                                                                                                   \
    }                                                                                                                  \
    static inline const cubos::core::reflection::Type& reflectMake()                                                   \
    {                                                                                                                  \
        return ::cubos::core::reflection::makeAnonymousType(                                                           \
            #__VA_ARGS__, __FILE__, sizeof(__VA_ARGS__), alignof(__VA_ARGS__),                                         \
            [](void* ptr) { static_cast<__VA_ARGS__*>(ptr)->~__VA_ARGS__(); });                                        \
    }                                                                                                                  \
    static_assert(true, "") /* Here just to force the user to enter a semicolon */
