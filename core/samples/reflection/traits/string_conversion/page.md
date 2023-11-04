# String Conversion Trait {#examples-core-reflection-traits-string-conversion}

@brief Exposing string conversion functions of a type.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/reflection/traits/string_conversion).

Lets say you have a type `Name`, which is just a wrapper around an
`std::string`. If you want it to be picked up by (de)serializers and other
reflection consumers as just a string, you can add the @ref
cubos::core::reflection::StringConversionTrait "StringConversionTrait" to your
type.

@snippet reflection/traits/string_conversion/main.cpp Name declaration

The trait's constructor takes two functions: one to convert an instance of the
to a string, and the other to convert a string to an instance of the type. The
first method never fails, but the second one should return a boolean indicating
whether the passed string was valid.

@snippet reflection/traits/string_conversion/main.cpp Name definition

Now, we can access the trait from the reflected type:

@snippet reflection/traits/string_conversion/main.cpp Accessing the trait

Using the trait is as simple as:

@snippet reflection/traits/string_conversion/main.cpp Converting to a string

@snippet reflection/traits/string_conversion/main.cpp Converting from a string
