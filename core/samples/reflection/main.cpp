/// @file
/// @brief Sample which showcases how to define a reflectable object type.
///
/// @details The reflection module allows you to define the structure of types, attach metadata to
/// fields, and query this information at runtime. This is very useful for serialization and GUI,
/// for example.

#include <cubos/core/memory/stream.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/external/variant.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/object.hpp>
#include <cubos/core/reflection/primitive.hpp>

#include "fruit.hpp"
#include "printer.hpp"

using cubos::core::memory::Stream;
using cubos::core::reflection::ObjectType;
using cubos::core::reflection::reflect;

int main()
{
    // Reflection data can be obtained using the reflect() function. This function returns a
    // reference to a Type object - a base class for all kinds of types. If you know the kind of
    // the type you are reflecting, you can use the asKind() function to cast the Type object to a
    // more specific kind, such as ObjectType.

    Stream::stdOut.print("--- Printing the fields of an object type using asKind() ---\n\n");

    auto& fruitType = reflect<Fruit>().asKind<ObjectType>(); // Would abort if Fruit was not an object type.
    Stream::stdOut.printf("Name: {}\n", fruitType.name());

    // Iterate over all fields of the object and print their names and types.
    Stream::stdOut.printf("Fields:\n");
    for (auto& field : fruitType.fields())
    {
        Stream::stdOut.printf("  {}: {}\n", field->name(), field->type().name());
    }

    // An alternative to casting is to implement the TypeVisitor interface. In this sample, we
    // implement a Printer visitor which prints the type data of all types it visits, recursively.

    Stream::stdOut.print("\n--- Printing the fields of types using a visitor ---\n\n");

    Printer printer{Stream::stdOut};
    fruitType.accept(printer);
    Stream::stdOut.put('\n');

    // std::variant's is also reflectable - the header <cubos/core/reflection/external/variant.hpp>
    // must be included to use it. The header <cubos/core/reflection/external/primitives.hpp> must
    // be included to reflect primitive types such as int and float.

    reflect<std::variant<int, float>>().accept(printer);
    Stream::stdOut.put('\n');

    // std::vector's can also be reflected by including <cubos/core/reflection/external/vector.hpp>.

    reflect<std::vector<Fruit>>().accept(printer);
    Stream::stdOut.put('\n');

    // std::unordered_map's too, by including <cubos/core/reflection/external/unordered_map.hpp>.
    reflect<std::unordered_map<int, bool>>().accept(printer);
    Stream::stdOut.put('\n');

    // All together now!
    auto& bigType = reflect<std::variant<std::vector<Fruit>, std::unordered_map<int, bool>>>();
    bigType.accept(printer);
    Stream::stdOut.put('\n');

    // When you have a template type and its name is too long, you can use the shortName() method
    // to get its name without the template arguments.
    Stream::stdOut.printf("Name: {}\nShort name: {}\n", bigType.name(), bigType.shortName());
}
