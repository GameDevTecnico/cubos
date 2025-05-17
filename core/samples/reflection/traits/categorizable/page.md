# Categorizable Trait {#examples-core-reflection-traits-categorizable}

@brief Categorizing a type. 

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/reflection/traits/categorizable).

There might be cases where grouping components would be a great idea.
The @ref cubos::core::reflection::CategorizableTrait "CategorizableTrait" trait
allows you to categorize a type and set a priority within its category. 

So, let's see how we can use this. First, we create two reflectable structures:

@snippet reflection/traits/categorizable/main.cpp Transform Entities declaration

We want both of the types to have the same category, `Transform`, but different priorities: 

@snippet reflection/traits/categorizable/main.cpp Reflection definition

Now, we can access the trait from the reflected type:

@snippet reflection/traits/categorizable/main.cpp Accessing the trait

We can then simply check the category and priority of the `Position` type.

@snippet reflection/traits/categorizable/main.cpp Checking the category and priority of a type

The same works for the `Scale` type, which uses the type builder and categorize method.

@snippet reflection/traits/categorizable/main.cpp Scale type 

