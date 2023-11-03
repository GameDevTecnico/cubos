# Custom Deserializer {#examples-core-data-des-custom}

@brief Implementing your own @ref cubos::core::data::Deserializer "Deserializer".

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/data/des/custom).

To define your own deserializer type, you'll need to include
@ref core/data/des/deserializer.hpp. For simplicity, in this sample we'll use
the following aliases:

@snippet data/des/custom/main.cpp Include

We'll define a deserializer that will print the data to the standard output.

@snippet data/des/custom/main.cpp Your own deserializer

In the constructor, we should set hooks to be called for deserializing primitive
types or any other type we want to handle specifically.

In this example, we'll only handle `int32_t`, but usually you should at least
cover all primitive types.

@snippet data/des/custom/main.cpp Setting up hooks

The only other thing you need to do is implement the @ref
cubos::core::data::deserializer::decompose "deserializer::decompose" method,
which acts as a catch-all for any type without a specific hook.

Here, we can use traits such as @ref cubos::core::reflection::FieldsTrait
"FieldsTrait" to access the fields of a type and write to them.

In this sample, we'll only be handling fields and arrays, but you should try to
cover as many kinds of data as possible.

@snippet data/des/custom/main.cpp Decomposing types

We start by checking if the type can be viewed as an array. If it can, we'll
ask the user how many elements they want the array to have. We resize it, and
then, we recurse into the elements. If the type doesn't have this trait, we'll
fallback into checking if it has fields.

@snippet data/des/custom/main.cpp Decomposing types with fields

If the type has fields, we'll iterate over them and ask the user to enter
values for them. Otherwise, we'll fail by returning `false`.

Using our deserializer is as simple as constructing it and calling @ref
cubos::core::data::Deserializer::read "Deserializer::read" on the data we want
to deserialize.

In this case, we'll be deserializing a `std::vector<glm::ivec3>`, which is
an array of objects with three `int32_t` fields.

@snippet data/des/custom/main.cpp Usage

This should output the values you enter when you execute it.
