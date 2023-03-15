#include <cubos/core/data/serializer.hpp>
#include <cubos/core/data/serialization_map.hpp>
#include <cubos/core/data/json_serializer.hpp>

using namespace cubos::core::memory;
using namespace cubos::core::data;

// Simple serializable type.
struct Fruit
{
    std::string name;
    int32_t weight;
};

// Simple serializable type with pointers.
struct Human
{
    Human* father;
    Human* mother;
    int32_t age;
    std::string name;
};

template <>
void cubos::core::data::serialize<Fruit>(Serializer& ser, const Fruit& fruit, const char* name)
{
    ser.beginObject(name);
    ser.write(fruit.name, "name");
    ser.write(fruit.weight, "weight");
    ser.endObject();
}

template <>
void cubos::core::data::serialize<Human>(Serializer& ser, const Human& human, const char* name)
{
    // Get necessary context from the serializer.
    auto& map = ser.context().get<SerializationMap<Human*, int>>();

    ser.beginObject(name);
    ser.write(map.getId(human.father), "father");
    ser.write(map.getId(human.mother), "mother");
    ser.write(human.age, "age");
    ser.write(human.name, "name");
    ser.endObject();
}

int main()
{
    // Output JSON to stdout
    auto ser = JSONSerializer(Stream::stdOut, 4);

    // Serialize a dictionary of fruits
    std::unordered_map<int, Fruit> fruitsByPrice = {
        {50, {"Apple", 100}}, {100, {"Orange", 200}}, {150, {"Banana", 300}}, {200, {"Pear", 120}}, {30, {"Grape", 20}},
    };

    Stream::stdOut.print("# Dictionary of trivially serializable types\n");
    Stream::stdOut.print("# ------------------------------------------\n");
    ser.write(fruitsByPrice, "fruitsByPrice");
    ser.flush();

    // Array to serialize
    std::vector<Human> humans(5);
    humans[0] = {nullptr, nullptr, 43, "John"};
    humans[1] = {&humans[0], &humans[2], 12, "Alex"};
    humans[2] = {nullptr, &humans[3], 41, "Jane"};
    humans[3] = {nullptr, nullptr, 82, "Emma"};
    humans[4] = {nullptr, &humans[4], 53, "Oliver"};

    // Prepare context (in this case its a SerializationMap, used to map references to ids)
    auto map = SerializationMap<Human*, int>();
    map.add(nullptr, -1);
    for (size_t i = 0; i < humans.size(); ++i)
        map.add(&humans[i], i);
    ser.context().push(std::move(map));

    // Serialize the humans array
    Stream::stdOut.print("\n\n");
    Stream::stdOut.print("# Family tree which requires context to be serialized\n");
    Stream::stdOut.print("# ---------------------------------------------------\n");
    ser.write(humans, "family");
    ser.flush();

    return 0;
}
