#include <gtest/gtest.h>
#include <cubos/memory/buffer_stream.hpp>
#include <cubos/memory/yaml_serializer.hpp>

TEST(Cubos_Memory_YAML_Serialization, Serialize_Primitives)
{
    using namespace cubos::memory;

    char buf[2048];
    auto stream = BufferStream(buf, sizeof(buf));
    {
        auto serializer = YAMLSerializer(stream);
        serializer.write(12, "named_integer");
        serializer.write(0.5f, "named_float");
        serializer.write(6.5, "named_double");
        serializer.write(true, "named_bool");
        serializer.write("string", "named_string");
        serializer.write("\n\t\r\b", "named_string_with_special_chars");
        serializer.write("anonymous string", nullptr);
    }
    stream.put('\0');

    const char* expected = "---\n"
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
    using namespace cubos::memory;

    char buf[2048];
    auto stream = BufferStream(buf, sizeof(buf));
    {
        auto serializer = (Serializer*)new YAMLSerializer(stream);

        std::vector<int64_t> vec = {1, 5, 8, 3};
        std::vector<std::vector<std::vector<int>>> vec3d = {{{1, 2}, {3, 4, 5}}, {{6, 7}, {8}}};
        const char* strs[] = {"one", "two", "three"};

        serializer->write<int64_t>(vec, "vector");
        serializer->beginArray(3, "strings");
        for (size_t i = 0; i < 3; ++i)
            serializer->write(strs[i], nullptr);
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
    using namespace cubos::memory;

    char buf[2048];
    auto stream = BufferStream(buf, sizeof(buf));
    {
        auto serializer = (Serializer*)new YAMLSerializer(stream);

        std::unordered_map<std::string, int64_t> map = {{"one", 1}, {"two", 2}, {"three", 3}};
        const char* strs[] = {"one", "two", "three"};
        std::unordered_map<std::string, std::unordered_map<std::string, int>> map2d = {
            {"one", {{"one", 1}, {"two", 2}}},
            {"two", {{"three", 3}, {"four", 4}}},
        };

        serializer->write(map, "map");
        serializer->beginDictionary(3, "strings");
        for (size_t i = 0; i < 3; ++i)
        {
            serializer->write(i, nullptr);
            serializer->write(strs[i], nullptr);
        }
        serializer->endDictionary();
        serializer->write(map2d, "map2d");

        delete serializer;
    }
    stream.put('\0');

    const char* expected = "---\n"
                           "map:\n"
                           "  three: 3\n"
                           "  two: 2\n"
                           "  one: 1\n"
                           "strings:\n"
                           "  0: one\n"
                           "  1: two\n"
                           "  2: three\n"
                           "map2d:\n"
                           "  two:\n"
                           "    four: 4\n"
                           "    three: 3\n"
                           "  one:\n"
                           "    two: 2\n"
                           "    one: 1\n"
                           "...\n";
    EXPECT_STREQ(expected, buf);
}

TEST(Cubos_Memory_YAML_Serialization, Serialize_GLM)
{
    using namespace cubos::memory;

    char buf[2048];
    auto stream = BufferStream(buf, sizeof(buf));
    {
        auto serializer = (Serializer*)new YAMLSerializer(stream);
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
