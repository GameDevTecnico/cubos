#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <gtest/gtest.h>

#include <cubos/core/data/serialization_map.hpp>
#include <cubos/core/data/yaml_serializer.hpp>
#include <cubos/core/memory/buffer_stream.hpp>

using namespace cubos::core::memory;
using namespace cubos::core::data;

struct Human
{
    std::string name;
    int age;
    double weight;
    bool dead;
    std::vector<Human*> children;
};

template <>
void cubos::core::data::serialize<Human>(Serializer& ser, const Human& obj, const char* name)
{
    ser.beginObject(name);
    ser.write(obj.name, "name");
    ser.write(obj.age, "age");
    ser.write(obj.weight, "weight");
    ser.write(obj.dead, "dead");
    ser.beginArray(obj.children.size(), "children");
    for (const auto& child : obj.children)
    {
        if (ser.context().has<SerializationMap<Human*, std::size_t>>())
        {
            auto& map = ser.context().get<SerializationMap<Human*, std::size_t>>();
            ser.write(static_cast<uint64_t>(map.getId(child)), "child");
        }
        else
        {
            ser.write(*child, "child");
        }
    }
    ser.endArray();
    ser.endObject();
}

TEST(Cubos_Memory_YAML_Serialization, Serialize_Primitives)
{
    char buf[2048];
    auto stream = BufferStream(buf, sizeof(buf));
    {
        auto serializer = YAMLSerializer(stream);
        serializer.write(static_cast<uint8_t>(1), "named_uint8");
        serializer.write(12, "named_integer");
        serializer.write(0.5F, "named_float");
        serializer.write(6.5, "named_double");
        serializer.write(true, "named_bool");
        serializer.write("string", "named_string");
        serializer.write("\n\t\r\b", "named_string_with_special_chars");
        serializer.write("anonymous string", nullptr);
    }
    stream.put('\0');

    const char* expected = "---\n"
                           "named_uint8: 1\n"
                           "named_integer: 12\n"
                           "named_float: 0.5\n"
                           "named_double: 6.5\n"
                           "named_bool: true\n"
                           "named_string: string\n"
                           "named_string_with_special_chars: \"\\n\\t\\r\\b\"\n"
                           "anonymous: anonymous string\n"
                           "...\n";
    EXPECT_STREQ(expected, buf);
}

TEST(Cubos_Memory_YAML_Serialization, Serialize_Array)
{
    char buf[2048];
    auto stream = BufferStream(buf, sizeof(buf));
    {
        auto* serializer = (Serializer*)new YAMLSerializer(stream);

        std::vector<int64_t> vec = {1, 5, 8, 3};
        std::vector<std::vector<std::vector<int>>> vec3d = {{{1, 2}, {3, 4, 5}}, {{6, 7}, {8}}};
        const char* strs[] = {"one", "two", "three"};

        serializer->write(vec, "vector");
        serializer->beginArray(3, "strings");
        for (auto& str : strs)
        {
            serializer->write(str, nullptr);
        }
        serializer->endArray();
        serializer->write(vec3d, "vector3d");

        delete serializer;
    }
    stream.put('\0');

    const char* expected = "---\n"
                           "vector:\n"
                           "  - 1\n"
                           "  - 5\n"
                           "  - 8\n"
                           "  - 3\n"
                           "strings:\n"
                           "  - one\n"
                           "  - two\n"
                           "  - three\n"
                           "vector3d:\n"
                           "  -\n"
                           "    -\n"
                           "      - 1\n"
                           "      - 2\n"
                           "    -\n"
                           "      - 3\n"
                           "      - 4\n"
                           "      - 5\n"
                           "  -\n"
                           "    -\n"
                           "      - 6\n"
                           "      - 7\n"
                           "    -\n"
                           "      - 8\n"
                           "...\n";
    EXPECT_STREQ(expected, buf);
}

TEST(Cubos_Memory_YAML_Serialization, Serialize_Dictionary)
{
    char buf[2048];
    auto stream = BufferStream(buf, sizeof(buf));
    {
        auto* serializer = (Serializer*)new YAMLSerializer(stream);
        const char* strs[] = {"one", "two", "three"};

        serializer->beginDictionary(3, "strings");
        for (std::size_t i = 0; i < 3; ++i)
        {
            serializer->write(static_cast<uint64_t>(i), nullptr);
            serializer->write(strs[i], nullptr);
        }
        serializer->endDictionary();

        delete serializer;
    }
    stream.put('\0');

    const char* expected = "---\n"
                           "strings:\n"
                           "  0: one\n"
                           "  1: two\n"
                           "  2: three\n"
                           "...\n";
    EXPECT_STREQ(expected, buf);
}

TEST(Cubos_Memory_YAML_Serialization, Serialize_GLM)
{
    char buf[2048];
    auto stream = BufferStream(buf, sizeof(buf));
    {
        auto* serializer = (Serializer*)new YAMLSerializer(stream);
        serializer->write(glm::vec2(1, 2), "fvec2");
        serializer->write(glm::vec3(1, 2, 3), "fvec3");
        serializer->write(glm::vec4(1, 2, 3, 4), "fvec4");
        serializer->write(glm::ivec2(1, 2), "ivec2");
        serializer->write(glm::ivec3(1, 2, 3), "ivec3");
        serializer->write(glm::ivec4(1, 2, 3, 4), "ivec4");
        serializer->write(glm::uvec2(1, 2), "uvec2");
        serializer->write(glm::uvec3(1, 2, 3), "uvec3");
        serializer->write(glm::uvec4(1, 2, 3, 4), "uvec4");
        serializer->write(glm::quat(1, 2, 3, 4), "quat");
        delete serializer;
    }
    stream.put('\0');

    const char* expected = "---\n"
                           "fvec2:\n"
                           "  x: 1\n"
                           "  y: 2\n"
                           "fvec3:\n"
                           "  x: 1\n"
                           "  y: 2\n"
                           "  z: 3\n"
                           "fvec4:\n"
                           "  x: 1\n"
                           "  y: 2\n"
                           "  z: 3\n"
                           "  w: 4\n"
                           "ivec2:\n"
                           "  x: 1\n"
                           "  y: 2\n"
                           "ivec3:\n"
                           "  x: 1\n"
                           "  y: 2\n"
                           "  z: 3\n"
                           "ivec4:\n"
                           "  x: 1\n"
                           "  y: 2\n"
                           "  z: 3\n"
                           "  w: 4\n"
                           "uvec2:\n"
                           "  x: 1\n"
                           "  y: 2\n"
                           "uvec3:\n"
                           "  x: 1\n"
                           "  y: 2\n"
                           "  z: 3\n"
                           "uvec4:\n"
                           "  x: 1\n"
                           "  y: 2\n"
                           "  z: 3\n"
                           "  w: 4\n"
                           "quat:\n"
                           "  w: 1\n"
                           "  x: 2\n"
                           "  y: 3\n"
                           "  z: 4\n"
                           "...\n";
    EXPECT_STREQ(expected, buf);
}

TEST(Cubos_Memory_YAML_Serialization, Serialize_Custom)
{
    using namespace cubos::core::memory;

    char buf[4096];
    auto stream = BufferStream(buf, sizeof(buf));
    {
        std::vector<Human> humans = {{"José", 90, 79, true, {}},
                                     {"Joana", 70, 60, false, {}},
                                     {"Josefina", 50, 100, false, {}},
                                     {"João", 30, 80, true, {}},
                                     {"Joaquim", 23, 70, false, {}}};

        humans[0].children.push_back(&humans[1]);
        humans[1].children.push_back(&humans[2]);
        humans[2].children.push_back(&humans[3]);
        humans[2].children.push_back(&humans[4]);

        auto map = SerializationMap<Human*, std::size_t>();
        for (std::size_t i = 0; i < humans.size(); ++i)
        {
            map.add(&humans[i], i);
        }

        auto* serializer = (Serializer*)new YAMLSerializer(stream);
        serializer->write(humans[0], "without_context");
        serializer->context().push(std::move(map));
        serializer->beginDictionary(humans.size(), "with_context");
        for (std::size_t i = 0; i < humans.size(); ++i)
        {
            serializer->write(static_cast<uint64_t>(i), nullptr);
            serializer->write(humans[i], nullptr);
        }
        serializer->endDictionary();
        delete serializer;
    }
    stream.put('\0');

    EXPECT_STREQ("---\n"
                 "without_context:\n"
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
                 "                []\n"
                 "with_context:\n"
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
                 "      []\n"
                 "...\n",
                 buf);
}
