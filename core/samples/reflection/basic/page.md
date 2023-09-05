# Basic Usage {#examples-core-reflection-basic}

@brief Defining and using reflectable types.

Lets say you have a type `Person`, which you want to be able to reflect. You
can declare it as reflectable using the macro @ref CUBOS_REFLECT, for example,
in your header, like this:

@snippet reflection/basic/main.cpp Person definition

The file @ref core/reflection/reflect.hpp is a very lightweight header which
you should include when declaring types as reflectable. It only defines the
reflection macros and the reflection function. Avoid including other
unnecessary reflection headers, which might be heavier, in order to reduce
compile times.

In your source file, you must define the reflection data for your type. This is
done through the @ref CUBOS_REFLECT_IMPL macro:

@snippet reflection/basic/main.cpp Person reflection

To access this reflection data, you should use the
@ref cubos::core::reflection::reflect function, which is also defined in the
@ref core/reflection/reflect.hpp header.

@snippet reflection/basic/main.cpp Accessing type name

Lets say you want to associate your own data to your types, to describe them
further. For example, imagine you're making a GUI editor for your game and you
which to display the fields of your types in a tree view, with different colors
for different types. You could associate colors to your types by defining a
trait:

@snippet reflection/basic/main.cpp Your own trait

Now, when you define your type reflection, you add your trait with the
@ref cubos::core::reflection::Type::with "Type::with" method.

@snippet reflection/basic/main.cpp Adding your own trait

To check if a type has a trait, you use the
@ref cubos::core::reflection::Type::has "Type::has" method.

@snippet reflection/basic/main.cpp Checking traits

To actually access the trait data, you use the
@ref cubos::core::reflection::Type::get "Type::get" method.

@snippet reflection/basic/main.cpp Accessing traits
