# Constructible Trait {#examples-core-reflection-traits-constructible}

@brief Exposing the destructor and constructors of a type.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/reflection/traits/constructible).

You may find it useful while working with type-erased data to be able to create
copies of the data, destroy it, or move it around. The
@ref cubos::core::reflection::ConstructibleTrait "ConstructibleTrait" trait
exposes the size, alignment, destructor and constructors of a type.

Lets say you have a type `Scale`, which you want to be able to reflect, with a
default value of `1.0`:

@snippet reflection/traits/constructible/main.cpp Scale declaration

We're going to add the @ref cubos::core::reflection::ConstructibleTrait
"ConstructibleTrait" trait to it, so that we can create instances of it at
runtime:

@snippet reflection/traits/constructible/main.cpp Scale definition

Now, we can access the trait from the reflected type:

@snippet reflection/traits/constructible/main.cpp Accessing the trait

Imagine for a moment that you don't know the type of the data you're working,
and you only have access to its reflection data through `scaleType`. If you
want to create a default instance of the type, you can call the default
constructor stored in the trait:

@snippet reflection/traits/constructible/main.cpp Creating a default instance

Don't forget to destroy the instance manually when you're done with it:

@snippet reflection/traits/constructible/main.cpp Destroying the instance

The constructible trait always stores the destructor of a type, and may
optionally contain any of the basic constructors (default, copy and move), or
even arbitrary custom constructors, which receive arguments of reflectable
types. For example, below, we're calling the custom constructor we previously
reflected.

@snippet reflection/traits/constructible/main.cpp Creating a custom instance
