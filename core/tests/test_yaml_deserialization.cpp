#include <gtest/gtest.h>
#include <cubos/core/memory/buffer_stream.hpp>
#include <cubos/core/data/yaml_deserializer.hpp>
#include <cubos/core/data/serialization_map.hpp>

using namespace cubos::core::data;
using namespace cubos::core::memory;

struct Human
{
    std::string name;
    int age;
    double weight;
    bool dead;
    std::vector<Human*> children;
};

template <>
void cubos::core::data::deserialize<Human>(Deserializer& des, Human& obj)
{
    des.beginObject();
    des.read(obj.name);
    des.read(obj.age);
    des.read(obj.weight);
    des.read(obj.dead);
    obj.children.resize(des.beginArray());
    for (auto& child : obj.children)
    {
        if (des.context().has<SerializationMap<Human*, std::size_t>>())
        {
            auto& map = des.context().get<SerializationMap<Human*, std::size_t>>();
            uint64_t id;
            des.read(id);
            child = map.getRef(static_cast<std::size_t>(id));
        }
        else
        {
            child = new Human();
            des.read(*child);
        }
    }
    des.endArray();
    des.endObject();
}

TEST(Cubos_Memory_YAML_Deserialization, Deserialize_Primitives)
{
    const char* yaml = "---\n"
                       "int8: -128\n"
                       "int16: -32768\n"
                       "int32: -2147483648\n"
                       "int64: -9223372036854775807\n"
                       "uint8: 255\n"
                       "uint16: 65535\n"
                       "uint32: 4294967295\n"
                       "uint64: 18446744073709551615\n"
                       "float: -3.402823e+38\n"
                       "double: -1.7976931348623157e+308\n"
                       "bool: true\n"
                       "string: \"Hello World\"\n";

    auto stream = BufferStream(yaml, strlen(yaml));
    auto* deserializer = (Deserializer*)new YAMLDeserializer(stream);

    int8_t int8;
    int16_t int16;
    int32_t int32;
    int64_t int64;
    uint8_t uint8;
    uint16_t uint16;
    uint32_t uint32;
    uint64_t uint64;
    float float32;
    double float64;
    bool boolean;
    std::string string;

    deserializer->read(int8);
    deserializer->read(int16);
    deserializer->read(int32);
    deserializer->read(int64);
    deserializer->read(uint8);
    deserializer->read(uint16);
    deserializer->read(uint32);
    deserializer->read(uint64);
    deserializer->read(float32);
    deserializer->read(float64);
    deserializer->read(boolean);
    deserializer->read(string);

    EXPECT_EQ(int8, -128);
    EXPECT_EQ(int16, -32768);
    EXPECT_EQ(int32, -2147483648);
    EXPECT_EQ(int64, -9223372036854775807LL);
    EXPECT_EQ(uint8, 255);
    EXPECT_EQ(uint16, 65535);
    EXPECT_EQ(uint32, 4294967295);
    EXPECT_EQ(uint64, 18446744073709551615U);
    EXPECT_EQ(float32, -3.402823e+38F);
    EXPECT_EQ(float64, -1.7976931348623157e+308);
    EXPECT_EQ(boolean, true);
    EXPECT_EQ(string, "Hello World");

    delete deserializer;
}

TEST(Cubos_Memory_YAML_Deserialization, Deserialize_Array)
{
    const char* yaml = "---\n"
                       "array1: [1, 2, 3, 4, 5]\n"
                       "array2:\n"
                       "  - 6\n"
                       "  - 7\n"
                       "  - 8\n"
                       "  - 9\n"
                       "  - 10\n"
                       "array2d:\n"
                       "  - [11, 12, 13, 14, 15]\n"
                       "  - [16, 17, 18, 19, 20]\n"
                       "  - [21, 22, 23, 24, 25]\n"
                       "...\n";

    auto stream = BufferStream(yaml, strlen(yaml));
    auto* deserializer = (Deserializer*)new YAMLDeserializer(stream);

    std::vector<int> array;

    deserializer->read(array);
    EXPECT_EQ(array.size(), 5U);
    EXPECT_EQ(array[0], 1);
    EXPECT_EQ(array[1], 2);
    EXPECT_EQ(array[2], 3);
    EXPECT_EQ(array[3], 4);
    EXPECT_EQ(array[4], 5);
    deserializer->read(array);
    EXPECT_EQ(array.size(), 5U);
    EXPECT_EQ(array[0], 6);
    EXPECT_EQ(array[1], 7);
    EXPECT_EQ(array[2], 8);
    EXPECT_EQ(array[3], 9);
    EXPECT_EQ(array[4], 10);

    std::vector<std::vector<int>> array2d;
    deserializer->read(array2d);

    EXPECT_EQ(array2d.size(), 3U);
    EXPECT_EQ(array2d[0].size(), 5U);
    EXPECT_EQ(array2d[0][0], 11);
    EXPECT_EQ(array2d[0][1], 12);
    EXPECT_EQ(array2d[0][2], 13);
    EXPECT_EQ(array2d[0][3], 14);
    EXPECT_EQ(array2d[0][4], 15);
    EXPECT_EQ(array2d[1].size(), 5U);
    EXPECT_EQ(array2d[1][0], 16);
    EXPECT_EQ(array2d[1][1], 17);
    EXPECT_EQ(array2d[1][2], 18);
    EXPECT_EQ(array2d[1][3], 19);
    EXPECT_EQ(array2d[1][4], 20);
    EXPECT_EQ(array2d[2].size(), 5U);
    EXPECT_EQ(array2d[2][0], 21);
    EXPECT_EQ(array2d[2][1], 22);
    EXPECT_EQ(array2d[2][2], 23);
    EXPECT_EQ(array2d[2][3], 24);
    EXPECT_EQ(array2d[2][4], 25);

    delete deserializer;
}

TEST(Cubos_Memory_YAML_Deserialization, Deserialize_Dictionaries)
{
    const char* yaml = "---\n"
                       "dict1:\n"
                       "  key1: value1\n"
                       "  key2: value2\n"
                       "  key3: value3\n"
                       "dict2:\n"
                       "  key1: value1\n"
                       "  key2: value2\n"
                       "  key3: value3\n"
                       "dictOfDictOfArray:\n"
                       "  key1:\n"
                       "    key1:\n"
                       "      - 1\n"
                       "      - 2\n"
                       "      - 3\n"
                       "      - 4\n"
                       "      - 5\n"
                       "    key2:\n"
                       "      - 6\n"
                       "  key2:\n"
                       "    key1:\n"
                       "      - 7\n";

    auto stream = BufferStream(yaml, strlen(yaml));
    auto* deserializer = (Deserializer*)new YAMLDeserializer(stream);

    std::unordered_map<std::string, std::string> dict1;
    std::unordered_map<std::string, std::string> dict2;
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<int>>> dictOfDictOfArray;

    deserializer->read(dict1);
    EXPECT_EQ(dict1.size(), 3U);
    EXPECT_EQ(dict1["key1"], "value1");
    EXPECT_EQ(dict1["key2"], "value2");
    EXPECT_EQ(dict1["key3"], "value3");

    deserializer->read(dict2);
    EXPECT_EQ(dict2.size(), 3U);
    EXPECT_EQ(dict2["key1"], "value1");
    EXPECT_EQ(dict2["key2"], "value2");
    EXPECT_EQ(dict2["key3"], "value3");

    deserializer->read(dictOfDictOfArray);
    EXPECT_EQ(dictOfDictOfArray.size(), 2U);
    EXPECT_EQ(dictOfDictOfArray["key1"].size(), 2U);
    EXPECT_EQ(dictOfDictOfArray["key1"]["key1"].size(), 5U);
    EXPECT_EQ(dictOfDictOfArray["key1"]["key1"][0], 1);
    EXPECT_EQ(dictOfDictOfArray["key1"]["key1"][1], 2);
    EXPECT_EQ(dictOfDictOfArray["key1"]["key1"][2], 3);
    EXPECT_EQ(dictOfDictOfArray["key1"]["key1"][3], 4);
    EXPECT_EQ(dictOfDictOfArray["key1"]["key1"][4], 5);
    EXPECT_EQ(dictOfDictOfArray["key1"]["key2"].size(), 1U);
    EXPECT_EQ(dictOfDictOfArray["key1"]["key2"][0], 6);
    EXPECT_EQ(dictOfDictOfArray["key2"].size(), 1U);
    EXPECT_EQ(dictOfDictOfArray["key2"]["key1"].size(), 1U);
    EXPECT_EQ(dictOfDictOfArray["key2"]["key1"][0], 7);

    delete deserializer;
}

TEST(Cubos_Memory_YAML_Deserialization, Deserialize_Custom)
{
    const char* yaml = "---\n"
                       "human:\n"
                       "  name: José\n"
                       "  age: 90\n"
                       "  weight: 79\n"
                       "  dead: true\n"
                       "  children:\n"
                       "    - name: Joana\n"
                       "      age: 70\n"
                       "      weight: 60\n"
                       "      dead: false\n"
                       "      children:\n"
                       "        - name: Josefina\n"
                       "          age: 50\n"
                       "          weight: 100\n"
                       "          dead: false\n"
                       "          children:\n"
                       "            - name: João\n"
                       "              age: 30\n"
                       "              weight: 80\n"
                       "              dead: true\n"
                       "              children:\n"
                       "                []\n"
                       "            - name: Joaquim\n"
                       "              age: 23\n"
                       "              weight: 70\n"
                       "              dead: false\n"
                       "              children:\n"
                       "                []\n";

    auto stream = BufferStream(yaml, strlen(yaml));
    auto* deserializer = (Deserializer*)new YAMLDeserializer(stream);

    Human human;
    deserializer->read(human);

    EXPECT_EQ(human.name, "José");
    EXPECT_EQ(human.age, 90);
    EXPECT_EQ(human.weight, 79);
    EXPECT_TRUE(human.dead);
    EXPECT_EQ(human.children.size(), 1U);
    EXPECT_EQ(human.children[0]->name, "Joana");
    EXPECT_EQ(human.children[0]->age, 70);
    EXPECT_EQ(human.children[0]->weight, 60);
    EXPECT_FALSE(human.children[0]->dead);
    EXPECT_EQ(human.children[0]->children.size(), 1U);
    EXPECT_EQ(human.children[0]->children[0]->name, "Josefina");
    EXPECT_EQ(human.children[0]->children[0]->age, 50);
    EXPECT_EQ(human.children[0]->children[0]->weight, 100);
    EXPECT_FALSE(human.children[0]->children[0]->dead);
    EXPECT_EQ(human.children[0]->children[0]->children.size(), 2U);
    EXPECT_EQ(human.children[0]->children[0]->children[0]->name, "João");
    EXPECT_EQ(human.children[0]->children[0]->children[0]->age, 30);
    EXPECT_EQ(human.children[0]->children[0]->children[0]->weight, 80);
    EXPECT_TRUE(human.children[0]->children[0]->children[0]->dead);
    EXPECT_EQ(human.children[0]->children[0]->children[0]->children.size(), 0U);
    EXPECT_EQ(human.children[0]->children[0]->children[1]->name, "Joaquim");
    EXPECT_EQ(human.children[0]->children[0]->children[1]->age, 23);
    EXPECT_EQ(human.children[0]->children[0]->children[1]->weight, 70);
    EXPECT_FALSE(human.children[0]->children[0]->children[1]->dead);
    EXPECT_EQ(human.children[0]->children[0]->children[1]->children.size(), 0U);

    delete deserializer;
}

TEST(Cubos_Memory_YAML_Deserialization, Deserialize_Custom_With_Context)
{
    const char* yaml = "humans:\n"
                       "  0:\n"
                       "    name: José\n"
                       "    age: 90\n"
                       "    weight: 79\n"
                       "    dead: true\n"
                       "    children:\n"
                       "      - 1\n"
                       "  1:\n"
                       "    name: Joana\n"
                       "    age: 70\n"
                       "    weight: 60\n"
                       "    dead: false\n"
                       "    children:\n"
                       "      - 2\n"
                       "  2:\n"
                       "    name: Josefina\n"
                       "    age: 50\n"
                       "    weight: 100\n"
                       "    dead: false\n"
                       "    children:\n"
                       "      - 3\n"
                       "      - 4\n"
                       "  3:\n"
                       "    name: João\n"
                       "    age: 30\n"
                       "    weight: 80\n"
                       "    dead: true\n"
                       "    children:\n"
                       "      []\n"
                       "  4:\n"
                       "    name: Joaquim\n"
                       "    age: 23\n"
                       "    weight: 70\n"
                       "    dead: false\n"
                       "    children:\n"
                       "      []\n";

    auto stream = BufferStream(yaml, strlen(yaml));
    auto* deserializer = (Deserializer*)new YAMLDeserializer(stream);

    std::vector<Human> humans;
    humans.resize(deserializer->beginDictionary());

    auto map = SerializationMap<Human*, std::size_t>();
    for (std::size_t i = 0; i < humans.size(); ++i)
    {
        map.add(&humans[i], i);
    }

    deserializer->context().push(std::move(map));

    for (std::size_t i = 0; i < humans.size(); ++i)
    {
        uint64_t id;
        deserializer->read(id);
        deserializer->read(humans[id]);
    }

    EXPECT_EQ(humans.size(), 5U);
    EXPECT_EQ(humans[0].name, "José");
    EXPECT_EQ(humans[0].age, 90);
    EXPECT_EQ(humans[0].weight, 79);
    EXPECT_TRUE(humans[0].dead);
    EXPECT_EQ(humans[0].children.size(), 1U);
    EXPECT_EQ(humans[0].children[0]->name, "Joana");
    EXPECT_EQ(humans[0].children[0]->age, 70);
    EXPECT_EQ(humans[0].children[0]->weight, 60);
    EXPECT_FALSE(humans[0].children[0]->dead);
    EXPECT_EQ(humans[0].children[0]->children.size(), 1U);
    EXPECT_EQ(humans[0].children[0]->children[0]->name, "Josefina");
    EXPECT_EQ(humans[0].children[0]->children[0]->age, 50);
    EXPECT_EQ(humans[0].children[0]->children[0]->weight, 100);
    EXPECT_FALSE(humans[0].children[0]->children[0]->dead);
    EXPECT_EQ(humans[0].children[0]->children[0]->children.size(), 2U);
    EXPECT_EQ(humans[0].children[0]->children[0]->children[0]->name, "João");
    EXPECT_EQ(humans[0].children[0]->children[0]->children[0]->age, 30);
    EXPECT_EQ(humans[0].children[0]->children[0]->children[0]->weight, 80);
    EXPECT_TRUE(humans[0].children[0]->children[0]->children[0]->dead);
    EXPECT_EQ(humans[0].children[0]->children[0]->children[0]->children.size(), 0U);
    EXPECT_EQ(humans[0].children[0]->children[0]->children[1]->name, "Joaquim");
    EXPECT_EQ(humans[0].children[0]->children[0]->children[1]->age, 23);
    EXPECT_EQ(humans[0].children[0]->children[0]->children[1]->weight, 70);
    EXPECT_FALSE(humans[0].children[0]->children[0]->children[1]->dead);
    EXPECT_EQ(humans[0].children[0]->children[0]->children[1]->children.size(), 0U);
}

TEST(Cubos_Memory_YAML_Deserialization, Deserialize_Multiple_Documents)
{
    const char* src = "---\n"
                      "x: 1\n"
                      "...\n"
                      "---\n"
                      "x: 2\n"
                      "...\n"
                      "---\n"
                      "x: 3\n"
                      "...\n";

    auto stream = BufferStream(src, strlen(src));
    auto* deserializer = (Deserializer*)new YAMLDeserializer(stream);

    int x;
    deserializer->read(x);
    EXPECT_EQ(x, 1);
    deserializer->read(x);
    EXPECT_EQ(x, 2);
    deserializer->read(x);
    EXPECT_EQ(x, 3);

    delete deserializer;
}
