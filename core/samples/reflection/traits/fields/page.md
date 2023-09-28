# Fields Trait {#examples-core-reflection-traits-fields}

@brief Exposing the fields of a type.

For structured types, like classes and structs, you might want to expose its
public fields using the @ref cubos::core::reflection::FieldsTrait "FieldsTrait"
trait. In this example, we'll expose the fields of the following type:

@snippet reflection/traits/fields/main.cpp Person declaration

In its reflection definition, we'll add the @ref
cubos::core::reflection::FieldsTrait "FieldsTrait" trait to it with each of the
fields we want to expose:

@snippet reflection/traits/fields/main.cpp Reflection definition

@note Make you sure you include the reflection declarations for the types of
the fields you expose! In this case, we need the @ref
core/reflection/external/primitives.hpp header.

Accessing this trait is the same as with any other trait:

@snippet reflection/traits/fields/main.cpp Accessing the trait

We can iterate over the fields of the type with it:

@snippet reflection/traits/fields/main.cpp Iterating over fields

This should output:

@snippet reflection/traits/fields/main.cpp Iterating over fields output

Its also possible to access the fields of an instance of the type, and iterate
over them:

@snippet reflection/traits/fields/main.cpp Iterating over fields with data
@snippet reflection/traits/fields/main.cpp Iterating over fields with data output

Its also possible to access the fields by name:

@snippet reflection/traits/fields/main.cpp Accessing fields by name
@snippet reflection/traits/fields/main.cpp Accessing fields by name output

