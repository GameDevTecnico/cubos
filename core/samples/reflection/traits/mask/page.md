# Mask Trait {#examples-core-reflection-traits-mask}

@brief Exposing a bit mask type.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/core/samples/reflection/traits/mask).

In this example, we'll explore how to expose a bit mask along with its bits using @ref cubos::core::reflection::MaskTrait.

@snippet reflection/traits/mask/main.cpp Mask declaration

@snippet reflection/traits/mask/main.cpp Reflection definition

Notice that we don't include the `None` type in the trait, as it doesn't correspond to a bit.

@snippet reflection/traits/mask/main.cpp Using the type

@snippet reflection/traits/mask/main.cpp Output
