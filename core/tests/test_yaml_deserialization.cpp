#include <gtest/gtest.h>
#include <cubos/memory/buffer_stream.hpp>
#include <cubos/memory/yaml_deserializer.hpp>
#include <cubos/memory/serialization_map.hpp>

using namespace cubos::memory;

struct Human
{
    std::string name;
    int age;
    double weight;
    bool dead;
    std::vector<Human*> children;

    void deserialize(Deserializer& s)
    {
        s.read(this->name);
        s.read(this->age);
        s.read(this->weight);
        s.read(this->dead);
        this->children.resize(s.beginArray(), nullptr);
        for (auto& child : this->children)
        {
            child = new Human();
            s.read(*child);
        }
        s.endArray();
    }

    void deserialize(Deserializer& s, SerializationMap<Human*, size_t>* map)
    {
        s.read(this->name);
        s.read(this->age);
        s.read(this->weight);
        s.read(this->dead);
        this->children.resize(s.beginArray());
        for (auto& child : this->children)
        {
            size_t id;
            s.read(id);
            child = map->getRef(id);
        }
        s.endArray();
    }
};

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
    auto deserializer = (Deserializer*)new YAMLDeserializer(stream);

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
    bool bool_;
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
    deserializer->read(bool_);
    deserializer->read(string);

    EXPECT_EQ(int8, -128);
    EXPECT_EQ(int16, -32768);
    EXPECT_EQ(int32, -2147483648);
    EXPECT_EQ(int64, -9223372036854775807ll);
    EXPECT_EQ(uint8, 255);
    EXPECT_EQ(uint16, 65535);
    EXPECT_EQ(uint32, 4294967295);
    EXPECT_EQ(uint64, 18446744073709551615u);
    EXPECT_EQ(float32, -3.402823e+38f);
    EXPECT_EQ(float64, -1.7976931348623157e+308);
    EXPECT_EQ(bool_, true);
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
    auto deserializer = (Deserializer*)new YAMLDeserializer(stream);

    std::vector<int> array;

    deserializer->read(array);
    EXPECT_EQ(array.size(), 5u);
    EXPECT_EQ(array[0], 1);
    EXPECT_EQ(array[1], 2);
    EXPECT_EQ(array[2], 3);
    EXPECT_EQ(array[3], 4);
    EXPECT_EQ(array[4], 5);
    deserializer->read(array);
    EXPECT_EQ(array.size(), 5u);
    EXPECT_EQ(array[0], 6);
    EXPECT_EQ(array[1], 7);
    EXPECT_EQ(array[2], 8);
    EXPECT_EQ(array[3], 9);
    EXPECT_EQ(array[4], 10);

    std::vector<std::vector<int>> array2d;
    deserializer->read(array2d);

    EXPECT_EQ(array2d.size(), 3u);
    EXPECT_EQ(array2d[0].size(), 5u);
    EXPECT_EQ(array2d[0][0], 11);
    EXPECT_EQ(array2d[0][1], 12);
    EXPECT_EQ(array2d[0][2], 13);
    EXPECT_EQ(array2d[0][3], 14);
    EXPECT_EQ(array2d[0][4], 15);
    EXPECT_EQ(array2d[1].size(), 5u);
    EXPECT_EQ(array2d[1][0], 16);
    EXPECT_EQ(array2d[1][1], 17);
    EXPECT_EQ(array2d[1][2], 18);
    EXPECT_EQ(array2d[1][3], 19);
    EXPECT_EQ(array2d[1][4], 20);
    EXPECT_EQ(array2d[2].size(), 5u);
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
    auto deserializer = (Deserializer*)new YAMLDeserializer(stream);

    std::unordered_map<std::string, std::string> dict1;
    std::unordered_map<std::string, std::string> dict2;
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<int>>> dictOfDictOfArray;

    deserializer->read(dict1);
    EXPECT_EQ(dict1.size(), 3u);
    EXPECT_EQ(dict1["key1"], "value1");
    EXPECT_EQ(dict1["key2"], "value2");
    EXPECT_EQ(dict1["key3"], "value3");

    deserializer->read(dict2);
    EXPECT_EQ(dict2.size(), 3u);
    EXPECT_EQ(dict2["key1"], "value1");
    EXPECT_EQ(dict2["key2"], "value2");
    EXPECT_EQ(dict2["key3"], "value3");

    deserializer->read(dictOfDictOfArray);
    EXPECT_EQ(dictOfDictOfArray.size(), 2u);
    EXPECT_EQ(dictOfDictOfArray["key1"].size(), 2u);
    EXPECT_EQ(dictOfDictOfArray["key1"]["key1"].size(), 5u);
    EXPECT_EQ(dictOfDictOfArray["key1"]["key1"][0], 1);
    EXPECT_EQ(dictOfDictOfArray["key1"]["key1"][1], 2);
    EXPECT_EQ(dictOfDictOfArray["key1"]["key1"][2], 3);
    EXPECT_EQ(dictOfDictOfArray["key1"]["key1"][3], 4);
    EXPECT_EQ(dictOfDictOfArray["key1"]["key1"][4], 5);
    EXPECT_EQ(dictOfDictOfArray["key1"]["key2"].size(), 1u);
    EXPECT_EQ(dictOfDictOfArray["key1"]["key2"][0], 6);
    EXPECT_EQ(dictOfDictOfArray["key2"].size(), 1u);
    EXPECT_EQ(dictOfDictOfArray["key2"]["key1"].size(), 1u);
    EXPECT_EQ(dictOfDictOfArray["key2"]["key1"][0], 7);

    delete deserializer;
}

TEST(Cubos_Memory_YAML_Deserialization, Deserialize_Trivially_Deserializable)
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
    auto deserializer = (Deserializer*)new YAMLDeserializer(stream);

    Human human;
    deserializer->read(human);

    EXPECT_EQ(human.name, "José");
    EXPECT_EQ(human.age, 90);
    EXPECT_EQ(human.weight, 79);
    EXPECT_TRUE(human.dead);
    EXPECT_EQ(human.children.size(), 1u);
    EXPECT_EQ(human.children[0]->name, "Joana");
    EXPECT_EQ(human.children[0]->age, 70);
    EXPECT_EQ(human.children[0]->weight, 60);
    EXPECT_FALSE(human.children[0]->dead);
    EXPECT_EQ(human.children[0]->children.size(), 1u);
    EXPECT_EQ(human.children[0]->children[0]->name, "Josefina");
    EXPECT_EQ(human.children[0]->children[0]->age, 50);
    EXPECT_EQ(human.children[0]->children[0]->weight, 100);
    EXPECT_FALSE(human.children[0]->children[0]->dead);
    EXPECT_EQ(human.children[0]->children[0]->children.size(), 2u);
    EXPECT_EQ(human.children[0]->children[0]->children[0]->name, "João");
    EXPECT_EQ(human.children[0]->children[0]->children[0]->age, 30);
    EXPECT_EQ(human.children[0]->children[0]->children[0]->weight, 80);
    EXPECT_TRUE(human.children[0]->children[0]->children[0]->dead);
    EXPECT_EQ(human.children[0]->children[0]->children[0]->children.size(), 0u);
    EXPECT_EQ(human.children[0]->children[0]->children[1]->name, "Joaquim");
    EXPECT_EQ(human.children[0]->children[0]->children[1]->age, 23);
    EXPECT_EQ(human.children[0]->children[0]->children[1]->weight, 70);
    EXPECT_FALSE(human.children[0]->children[0]->children[1]->dead);
    EXPECT_EQ(human.children[0]->children[0]->children[1]->children.size(), 0u);

    delete deserializer;
}

TEST(Cubos_Memory_YAML_Deserialization, Deserialize_Context_Deserializable)
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
    auto deserializer = (Deserializer*)new YAMLDeserializer(stream);

    std::vector<Human> humans;
    humans.resize(deserializer->beginDictionary());

    auto map = SerializationMap<Human*, size_t>();
    for (size_t i = 0; i < humans.size(); ++i)
        map.add(&humans[i], i);

    for (size_t i = 0, id; i < humans.size(); ++i)
    {
        deserializer->read(id);
        deserializer->read(humans[id], &map);
    }

    EXPECT_EQ(humans.size(), 5u);
    EXPECT_EQ(humans[0].name, "José");
    EXPECT_EQ(humans[0].age, 90);
    EXPECT_EQ(humans[0].weight, 79);
    EXPECT_TRUE(humans[0].dead);
    EXPECT_EQ(humans[0].children.size(), 1u);
    EXPECT_EQ(humans[0].children[0]->name, "Joana");
    EXPECT_EQ(humans[0].children[0]->age, 70);
    EXPECT_EQ(humans[0].children[0]->weight, 60);
    EXPECT_FALSE(humans[0].children[0]->dead);
    EXPECT_EQ(humans[0].children[0]->children.size(), 1u);
    EXPECT_EQ(humans[0].children[0]->children[0]->name, "Josefina");
    EXPECT_EQ(humans[0].children[0]->children[0]->age, 50);
    EXPECT_EQ(humans[0].children[0]->children[0]->weight, 100);
    EXPECT_FALSE(humans[0].children[0]->children[0]->dead);
    EXPECT_EQ(humans[0].children[0]->children[0]->children.size(), 2u);
    EXPECT_EQ(humans[0].children[0]->children[0]->children[0]->name, "João");
    EXPECT_EQ(humans[0].children[0]->children[0]->children[0]->age, 30);
    EXPECT_EQ(humans[0].children[0]->children[0]->children[0]->weight, 80);
    EXPECT_TRUE(humans[0].children[0]->children[0]->children[0]->dead);
    EXPECT_EQ(humans[0].children[0]->children[0]->children[0]->children.size(), 0u);
    EXPECT_EQ(humans[0].children[0]->children[0]->children[1]->name, "Joaquim");
    EXPECT_EQ(humans[0].children[0]->children[0]->children[1]->age, 23);
    EXPECT_EQ(humans[0].children[0]->children[0]->children[1]->weight, 70);
    EXPECT_FALSE(humans[0].children[0]->children[0]->children[1]->dead);
    EXPECT_EQ(humans[0].children[0]->children[0]->children[1]->children.size(), 0u);
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
    auto deserializer = (Deserializer*)new YAMLDeserializer(stream);

    int x;
    deserializer->read(x);
    EXPECT_EQ(x, 1);
    deserializer->read(x);
    EXPECT_EQ(x, 2);
    deserializer->read(x);
    EXPECT_EQ(x, 3);

    delete deserializer;
}
