# Enum Trait {#examples-core-reflection-traits-enum}

@brief Exposing an enumeration with variants.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/reflection/traits/enum).

In this example, we'll explore how to expose an enumeration along with its variants using @ref cubos::core::reflection::EnumTrait.

@snippet reflection/traits/enum/main.cpp Enum declaration

@snippet reflection/traits/enum/main.cpp Reflection definition

This section includes the necessary headers for reflection and defines the reflection for the `Color` enumeration.

Output:

@snippet reflection/traits/enum/main.cpp Using the type

```txt
[2023-11-19 13:09:36.117] [info] [main.cpp:59] Variant 'Red'
[2023-11-19 13:09:36.118] [info] [main.cpp:59] Variant 'Green'
[2023-11-19 13:09:36.119] [info] [main.cpp:59] Variant 'Blue'
[2023-11-19 13:09:36.119] [info] [main.cpp:62] 'Red'
[2023-11-19 13:09:36.119] [info] [main.cpp:65] 'Green'
```
