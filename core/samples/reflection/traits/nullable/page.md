# Nullable Trait {#examples-core-reflection-traits-nullable}

@brief Handling null type representation.

The `NullableTrait` is a reflection trait designed to provide a mechanism for representing a null state for a specific type. This is particularly useful when working with types that are not literally null but can be conceptually treated as null in certain scenarios.

Consider a scenario where you have a data structure, such as the `Entity` struct, which represents an entity with an `index` and a `generation`. In some cases, you may want to define a special state that indicates the absence of a valid entity instead of checking if both fields are `UINT32_MAX`. So, instead of using a separate boolean flag to represent this state, the `NullableTrait` allows you to define a custom condition for considering an instance of the type as "null".

So, let's see how we can use this. First, we create a reflectable structure:

@snippet reflection/traits/nullable/main.cpp MyEntity declaration

In the reflection definition, we use the `NullableTrait` to define the conditions under which an instance of `MyEntity` is considered null, and also another to set it to null.

@snippet reflection/traits/nullable/main.cpp Reflection definition

Now, we can simply use it:

@snippet reflection/traits/nullable/main.cpp Example usage
