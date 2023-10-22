# Dictionary Trait {#examples-core-reflection-traits-dictionary}

@brief Exposing and using dictionary functionality of a type.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/reflection/traits/dictionary).

The @ref cubos::core::reflection::DictionaryTrait "DictionaryTrait" trait is
used to expose the dictionary functionality of a type. In this example, we will
write a function which takes a type and an instance of that type, and prints
its entries:

@snippet reflection/traits/dictionary/main.cpp Printing any dictionary

Through the trait, we can access the size of the dictionary and its key and
value types:

@snippet reflection/traits/dictionary/main.cpp Getting dictionary length and types

We can also iterate over the entries of a dictionary and access them:

@snippet reflection/traits/dictionary/main.cpp Getting dictionary entries

In this example, we're only supporting dictionaris which map `int32_t`s, but we
could for example implement a printing function which supports all primitive
types.

To make calling our function easier, we can add a convenience typed wrapper:

@snippet reflection/traits/dictionary/main.cpp Typed wrapper

Using this function is now as simple as:

@snippet reflection/traits/dictionary/main.cpp Usage

Its important to note that both the includes above are necessary, as we're
reflecting the type `std::map<int32_t, int32_t>`, which also means reflecting
`int32_t`.

Executing the sample should output:

@snippet reflection/traits/dictionary/main.cpp Expected output
