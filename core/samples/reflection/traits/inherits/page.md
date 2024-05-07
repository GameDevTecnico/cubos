# Inherits Trait {#examples-core-reflection-traits-inherits}

@brief Exposing the inheritance relationship between types.

For structured types, you might want to expose their inheritance relationships using `InheritsTrait`. This trait allows you to specify the parent type from which another type inherits.

In this example, we have three simple structs, `GrandParent`, `Parent` and `Son`, where `Son` is declared to inherit from `Parent`, and parent from `GrandParent`.

@snippet reflection/traits/inherits/main.cpp Parent and Son declaration

@snippet reflection/traits/inherits/main.cpp Reflection definition

@snippet reflection/traits/inherits/main.cpp Test function

Output:
```sh
[15:30:10.611] [main.cpp:56 reflectType] info: "Son" inherits from Parent
[15:30:10.611] [main.cpp:65 reflectType] info: "Son" inherits from GrandParent
[15:30:10.611] [main.cpp:60 reflectType] info: "Parent" DOES NOT inherit from Parent
[15:30:10.611] [main.cpp:65 reflectType] info: "Parent" inherits from GrandParent
[15:30:10.611] [main.cpp:60 reflectType] info: "int" DOES NOT inherit from Parent
[15:30:10.611] [main.cpp:69 reflectType] info: "int" DOES NOT inherit from GrandParent
```