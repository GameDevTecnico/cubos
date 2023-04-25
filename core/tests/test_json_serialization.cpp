#include <gtest/gtest.h>

#include <cubos/core/data/json_serializer.hpp>
#include <cubos/core/memory/buffer_stream.hpp>

using namespace cubos::core::memory;
using namespace cubos::core::data;

TEST(Cubos_Memory_JSON_Serialization, Serialize_Primitives)
{
    char buf[2048];
    auto stream = BufferStream(buf, sizeof(buf));
    {
        auto serializer = JSONSerializer(stream, 1);
        serializer.beginObject(nullptr);
        serializer.write(static_cast<uint8_t>(1), "uint8");
        serializer.write(12, "integer");
        serializer.write(0.5F, "float");
        serializer.write(6.5, "double");
        serializer.write(true, "bool");
        serializer.write("string", "string");
        serializer.write("\n\t\r\b", "string_with_special_chars");
        serializer.endObject();
    }
    stream.put('\0');

    const char* expected = "{\n"
                           " \"uint8\": 1,\n"
                           " \"integer\": 12,\n"
                           " \"float\": 0.5,\n"
                           " \"double\": 6.5,\n"
                           " \"bool\": true,\n"
                           " \"string\": \"string\",\n"
                           " \"string_with_special_chars\": \"\\n\\t\\r\\b\"\n"
                           "}";
    EXPECT_STREQ(expected, buf);
}

TEST(Cubos_Memory_JSON_Serialization, Serialize_Array)
{
    char buf[2048];
    auto stream = BufferStream(buf, sizeof(buf));
    {
        auto serializer = JSONSerializer(stream, 1);

        std::vector<int64_t> vec = {1, 5, 8, 3};
        std::vector<std::vector<std::vector<int>>> vec3d = {{{1, 2}, {3, 4, 5}}, {{6, 7}, {8}}};
        const char* strs[] = {"one", "two", "three"};

        serializer.beginObject(nullptr);
        serializer.write(vec, "vector");
        serializer.beginArray(3, "strings");
        for (auto& str : strs)
        {
            serializer.write(str, nullptr);
        }
        serializer.endArray();
        serializer.write(vec3d, "vector3d");
        serializer.endObject();
    }
    stream.put('\0');

    const char* expected = "{\n"
                           " \"vector\": [\n"
                           "  1,\n"
                           "  5,\n"
                           "  8,\n"
                           "  3\n"
                           " ],\n"
                           " \"strings\": [\n"
                           "  \"one\",\n"
                           "  \"two\",\n"
                           "  \"three\"\n"
                           " ],\n"
                           " \"vector3d\": [\n"
                           "  [\n"
                           "   [\n"
                           "    1,\n"
                           "    2\n"
                           "   ],\n"
                           "   [\n"
                           "    3,\n"
                           "    4,\n"
                           "    5\n"
                           "   ]\n"
                           "  ],\n"
                           "  [\n"
                           "   [\n"
                           "    6,\n"
                           "    7\n"
                           "   ],\n"
                           "   [\n"
                           "    8\n"
                           "   ]\n"
                           "  ]\n"
                           " ]\n"
                           "}";

    EXPECT_STREQ(expected, buf);
}

TEST(Cubos_Memory_JSON_Serialization, Serialize_Dictionary)
{
    char buf[2048];
    auto stream = BufferStream(buf, sizeof(buf));
    {
        auto serializer = JSONSerializer(stream);
        const char* strs[] = {"one", "two", "three"};

        serializer.beginDictionary(3, "strings");
        for (std::size_t i = 0; i < 3; ++i)
        {
            serializer.write(static_cast<uint64_t>(i), nullptr);
            serializer.write(strs[i], nullptr);
        }
        serializer.endDictionary();
    }
    stream.put('\0');

    const char* expected = R"({"0":"one","1":"two","2":"three"})";
    EXPECT_STREQ(expected, buf);
}
