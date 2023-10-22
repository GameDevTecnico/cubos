# Array Trait {#examples-core-reflection-traits-array}

@brief Exposing and using array functionality of a type.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/reflection/traits/array).

The @ref cubos::core::reflection::ArrayTrait "ArrayTrait" trait is used to
expose the array functionality of a type. In this example, we will write a
function which takes a type and an instance of that type, and prints its
elements:

@snippet reflection/traits/array/main.cpp Printing any array

Through the trait, we can access the size of the array and its element type:

@snippet reflection/traits/array/main.cpp Getting array length and type

We can also get pointers to the elements of the array and iterate over them:

@snippet reflection/traits/array/main.cpp Getting array elements

In this example, we're only supporting arrays of `int32_t`s, but we could for
example implement a printing function which supports all primitive types.

To make calling our function easier, we can add a convenience typed wrapper:

@snippet reflection/traits/array/main.cpp Typed wrapper

Using this function is now as simple as:

@snippet reflection/traits/array/main.cpp Usage

Its important to note that both the includes above are necessary, as we're
reflecting the type `std::vector<int32_t>`, which also means reflecting
`int32_t`.

Executing the sample should output:

@snippet reflection/traits/array/main.cpp Expected output
