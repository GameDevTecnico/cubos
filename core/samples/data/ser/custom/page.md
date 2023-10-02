# Custom Serializer {#examples-core-data-ser-custom}

@brief Implementing your own @ref cubos::core::data::Serializer "Serializer".

To define your own serializer type, you'll need to include
@ref core/data/ser/serializer.hpp. For simplicity, in this sample we'll use
the following aliases:

@snippet data/ser/custom/main.cpp Include

We'll define a serializer that will print the data to the standard output.

@snippet data/ser/custom/main.cpp Your own serializer

In the constructor, we should set hooks to be called for serializing primitive
types or any other type we want to handle specifically.

In this example, we'll only handle `int32_t`, but usually you should at least
cover all primitive types.

@snippet data/ser/custom/main.cpp Setting up hooks

The only other thing you need to do is implement the @ref
cubos::core::data::Serializer::decompose "Serializer::decompose" method, which
acts as a catch-all for any type without a specific hook.

Here, we can use traits such as @ref cubos::core::reflection::FieldsTrait
"FieldsTrait" to get the fields of a type and print them.

In this sample, we'll only be handling fields and arrays, but you should try to
cover as many kinds of data as possible.

@snippet data/ser/custom/main.cpp Decomposing types

We start by checking if the type can be viewed as an array. If it can, we
recurse into its elements.
Otherwise, we'll fallback to the fields of the type.

@snippet data/ser/custom/main.cpp Decomposing types with fields

If the type has fields, we'll iterate over them and print them.
Otherwise, we'll fail by returning `false`.

Using our serializer is as simple as constructing it and calling @ref
cubos::core::data::Serializer::write "Serializer::write" on the data we want to
serialize.

In this case, we'll be serializing a `std::vector<glm::ivec3>`, which is
an array of objects with three `int32_t` fields.

@snippet data/ser/custom/main.cpp Usage

This should output:

@snippet data/ser/custom/main.cpp Output
