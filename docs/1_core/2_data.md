# Data {#core-data}

The namespace \ref cubos::core::data provides utilities for
interacting with files and also provides a serialization system.

## File System

TODO

## Serialization

[Wikipedia](https://en.wikipedia.org/wiki/Serialization) defines serialization
as:

> the process of translating a data structure or object state into a format
> can be stored (for example, in a file or memory data buffer) or transmitted
> (for example, over a computer network) and reconstructed later (possibly in
> a different computer environment).

How does this fit into **CUBOS.**? Well, lets say we have defined the following
structure:
```cpp
struct Fruit
{
    std::string name;
    float weight;
    uint32_t price;
};
```

What if we wanted to load this data from a file? Or save it to a file? Or even
print it to the console? Our serialization system is designed to make this
easy. First, we need to make the type serializable and deserializable. For
that, we just need to:
- include the serializer and deserializer headers.
- define a function for writing (serializing).
- define a function for reading (deserializing).

```cpp
#include <cubos/core/data/serializer.hpp>
#include <cubos/core/data/deserializer.hpp>

... // struct Fruit definition

// We must make sure that the functions are defined in this namespace, even if
// Fruit is defined in another namespace.
namespace cubos::core::data
{
    void serialize(Serializer& s, const Fruit& f, const char* name)
    {
        s.beginObject(name);
        s.write(f.name, "name");
        s.write(f.weight, "weight");
        s.write(f.price, "price");
        s.endObject();
    }

    void deserialize(Deserializer& d, Fruit& f)
    {
        d.beginObject();
        d.read(f.name);
        d.read(f.weight);
        d.read(f.price);
        d.endObject();
    }
}
```

The `serialize` function is responsible for writing the data to a generic
\ref cubos::core::data::Serializer "Serializer", while the `deserialize`
function is responsible for reading the data from a generic
\ref cubos::core::data::Deserializer "Deserializer".

In the serialization function, we specify that we are writing an object, and
then we write the fields of the object. In the deserialization function, we
specify that we are reading an object, and then we read the fields of the
object, which must be done in the same order as in the serialization function.
 
The main advantage of this approach is that we have just written two simple
functions, and now we are able to write and read `Fruit` objects to and from a
multitude of different formats. At the moment of writing this, we have two
\ref cubos::core::data::Serializer "Serializer" and
\ref cubos::core::data::Deserializer "Deserializer" implementations:
\ref cubos::core::data::YAMLSerializer "YAMLSerializer" and
\ref cubos::core::data::YAMLDeserializer "YAMLDeserializer", and
\ref cubos::core::data::BinarySerializer "BinarySerializer" and
\ref cubos::core::data::BinarySerializer "BinarySerializer".

Let's say we would like to write a list of `Fruit` objects to the console in
YAML format. First we'll need to create a
\ref cubos::core::data::YAMLSerializer "YAMLSerializer":
```cpp
// The include cubos/core/data/yaml_serializer.hpp is necessary.
using namespace cubos::core;
auto serializer = data::YAMLSerializer(memory::Stream::stdOut);
...
```

We've just created a \ref cubos::core::data::YAMLSerializer "YAMLSerializer"
object, which outputs to the standard output stream. Now, let's write a list
of `Fruit` objects, and some more test data:
```cpp
...
std::vector<Fruit> fruits = {
    Fruit { "Apple", 0.5f, 10 },
    Fruit { "Orange", 0.3f, 20 },
    Fruit { "Banana", 0.2f, 30 },
};

serializer.write(fruits, "fruits");
serializer.write(42, "number");
```

Since `Fruit` and `std::vector<T>` are both serializable (for `Fruit`, we
defined a `serialize` function, and `std::vector<T>` has one defined
automatically for every `T` which is serialiable), we can just use
\ref cubos::core::data::Serializer::write "Serializer::write".

This piece of code should output the following to the console:
```yaml
---
fruits:
- name: Apple
  weight: 0.5
  price: 10
- name: Orange
  weight: 0.3
  price: 20
- name: Banana
  weight: 0.2
  price: 30
number: 42
...
```

Until now we've only written data. What if we wanted to read the data back?
Let's now create a \ref cubos::core::data::YAMLDeserializer "YAMLDeserializer" object, which reads from the standard
input stream:
```cpp
auto deserializer = data::YAMLDeserializer(memory::Stream::stdIn);
...
```

For the deserialization, we can just use
\ref cubos::core::data::Deserializer::read "Deserializer::read", because, once
again, `Fruit` and `std::vector<T>` are both deserializable.

```cpp
...
std::vector<Fruit> fruits;
int number;

deserializer.read(fruits);
deserializer.read(number);
```

Now, if you run this code, and feed it the output of the previous code, you
should get the same list of fruits and number back.

### Context serializable types

Until now, we've only covered trivially serializable types: they can be
serialized without providing any additional context. There are however,
some types which do require additional context. Imagine we want to serialize
the following structure:
```cpp
struct Human
{
    std::string name;
    Human* mother;
    Human* father;
};

using Family = std::vector<Human>;
```

Lets say that any of the members of the `Human` struct can be null, but that
when they're not, they always point to a valid `Human` object within the same
`Family` object.

A valid family, would be, for example:
```cpp
Family family;
family.resize(4);
family[0] = { "John", nullptr, nullptr };
family[1] = { "Jane", nullptr, nullptr };
family[2] = { "Jack", &family[0], &family[1] };
family[3] = { "Jill", &family[0], &family[1] };
// John and Jane have two children, Jack and Jill.
```

How would we define a serialization function for this type?
The first thing that comes to mind is to recursively serialize the father and
mother of each human, and then serialize its name. Then, if `Human` is
serializable, we could just do `serializer.write(family, "family");`.
*Wrong.* If we serialized it to YAML, we would get something like this:
```yaml
---
family:
- name: John
  mother: null
  father: null
- name: Jane
  mother: null
  father: null
- name: Jack
  mother:
    name: John
    mother: null
    father: null
  father:
    name: Jane
    mother: null
    father: null
- name: Jill
  mother: ...
...
```

As you've probably noticed, there's a lot of redundant information in this
example. This is because we serialized the `mother` and `father` members of
John, even though they had already been serialized.

If we instead, provide extra information to the serializer, we can serialize
this in a more compact way - lets associate an index to each address. There
is a built-in utility for situations like this: the
\ref cubos::core::data::SerializationMap "SerializationMap". This utility will
be our extra context.

This utility maps 'references' to 'identifiers', where the references
represent the original object references (in our case, the pointers), and the
identifiers represent the identifiers which are actually serialized.
For this example, we could generate those mappings like this:

```cpp
// Lets map Human* to identifiers.
auto map = SerializationMap<Human*, int32_t>();
map.add(nullptr, -1); // Lets map nullptr to -1.
for (size_t i = 0; i < family.size(); ++i)
    map.add(&family[i], i); // Add a mapping index <-> Human*

// Then we serialize the family, using the map.
serializer.write(family, &map, "family");
```

The serialization functions for `Human` then could be written as:

```cpp
// Once again, we must put these in the right namespace.
namespace cubos::core::data
{
    void serialize(Serializer& s, const Human& h, SerializationMap<Human*, uint32_t>* map, const char* name)
    {
        s.beginObject(name);
        s.write(h.name, "name");
        s.write(map->getId(h.mother), "mother");
        s.write(map->getId(h.father), "father");
        s.endObject();
    }

    void deserialize(Deserializer& d, Human& h, SerializationMap<Human*, uint32_t>* map)
    {
        int32_t id;

        d.beginObject();
        d.read(h.name);
        d.read(id);
        h.mother = map->getRef(id); // Here we do the opposite, from id to Human*        
        d.read(id);
        h.father = map->getRef(id);
        d.endObject();
    }
}
```

With this, serializing the family to YAML should output the following:
```yaml
---
family:
- name: John
  mother: -1
  father: -1
- name: Jane
  mother: -1
  father: -1
- name: Jack
  mother: 0
  father: 1
- name: Jill
  mother: 0
  father: 1
...
```

Which contains exactly the information we need to reconstruct the family!
You can check the \ref core `serialization.cpp` sample for more details. 

### Going further

One way we're taking advantage of the serialization system is to use it to both
show object's properties in the UI (with `ImGui`) and also allow the user to
edit those properties.

For this purpose, we needed an easy way to inspect properties of any
serializable object, and we also needed a way to edit those properties. So, we
came up with the \ref cubos::core::data::Package "Package" class. Any object
which provides a trivial serialization function (that is, a function which
won't receive extra arguments) can be packaged. We can also unpackage packages
into trivially deserializable objects!

Previously we defined a trivially serializable `Fruit` struct. Let's say we
wanted to inspect which properties a `Fruit` object has, dynamically. We can
first package it:

```cpp
Fruit fruit = { "Apple", 0.5f, 10 };
auto pkg = Package::from(fruit);
...
```

We can then inspect its fields:

```cpp
...
std::cout << pkg.size() << "\n"; // 3 fields
std::cout << pkg.field("name").get<std::string>() << "\n"; // Apple
std::cout << pkg.field("weight").get<float>() << "\n"; // 0.5f
std::cout << pkg.field("price").get<int32_t>() << "\n"; // 10
```

And even edit them if necessary:

```cpp
pkg.field("name").set("Banana");
...
```

If we edit the package, the original object won't be immediately updated. To
do this, we need to use the \ref cubos::core::data::Package::into "into" method.
This method only works with trivially deserializable objects, and will return
true or false, depending on whether the conversion was successful.

```cpp
Fruit fruit = { "Apple", 0.5f, 10 };
auto pkg = Package::from(fruit);
pkg.field("name").set("Banana");
pkg.into(fruit); // Writes the package contents into the fruit object.
std::cout << fruit.name << "\n"; // Banana
```
