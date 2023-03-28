#include <gtest/gtest.h>

#include <cubos/core/data/json_deserializer.hpp>

using namespace cubos::core::data;

TEST(Cubos_Memory_JSON_Deserialization, Deserialize_Primitives)
{
    auto deserializer = JSONDeserializer(R"(
        {
            "int8": -128,
            "int16": -32768,
            "int32": -2147483648,
            "int64": -9223372036854775807,
            "uint8": 255,
            "uint16": 65535,
            "uin32": 4294967295,
            "uint64": 18446744073709551615,
            "f32": -3.402823e+38,
            "f64": -1.7976931348623157e+308,
            "bool": true,
            "string": "Hello World"
        }
    )");

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

    deserializer.beginObject();
    deserializer.read(int8);
    deserializer.read(int16);
    deserializer.read(int32);
    deserializer.read(int64);
    deserializer.read(uint8);
    deserializer.read(uint16);
    deserializer.read(uint32);
    deserializer.read(uint64);
    deserializer.read(float32);
    deserializer.read(float64);
    deserializer.read(bool_);
    deserializer.read(string);
    deserializer.endObject();

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
}

TEST(Cubos_Memory_JSON_Deserialization, Deserialize_Array)
{
    auto deserializer = JSONDeserializer(R"(
        {
            "array1": [1, 2, 3, 4, 5],
            "array2": [6, 7, 8, 9, 10],
            "array2d": [
                [11, 12, 13, 14, 15],
                [16, 17, 18, 19, 20],
                [21, 22, 23, 24, 25]
            ]
        }
    )");

    deserializer.beginObject();

    std::vector<int> array;

    deserializer.read(array);
    EXPECT_EQ(array.size(), 5u);
    EXPECT_EQ(array[0], 1);
    EXPECT_EQ(array[1], 2);
    EXPECT_EQ(array[2], 3);
    EXPECT_EQ(array[3], 4);
    EXPECT_EQ(array[4], 5);

    deserializer.read(array);
    EXPECT_EQ(array.size(), 5u);
    EXPECT_EQ(array[0], 6);
    EXPECT_EQ(array[1], 7);
    EXPECT_EQ(array[2], 8);
    EXPECT_EQ(array[3], 9);
    EXPECT_EQ(array[4], 10);

    std::vector<std::vector<int>> array2d;
    deserializer.read(array2d);
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

    deserializer.endObject();
}

TEST(Cubos_Memory_JSON_Deserialization, Deserialize_Dictionaries)
{
    auto deserializer = JSONDeserializer(R"(
        {
            "dict": {
                "key1": "value1",
                "key2": "value2",
                "key3": "value3"
            },
            "dictOfDictOfArray": {
                "key1": {
                    "key1": [1, 2, 3, 4, 5],
                    "key2": [6]
                },
                "key2": {
                    "key1": [7]
                }
            }
        }
    )");

    std::unordered_map<std::string, std::string> dict;
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<int>>> dictOfDictOfArray;

    deserializer.beginObject();

    deserializer.read(dict);
    EXPECT_EQ(dict.size(), 3u);
    EXPECT_EQ(dict["key1"], "value1");
    EXPECT_EQ(dict["key2"], "value2");
    EXPECT_EQ(dict["key3"], "value3");

    deserializer.read(dictOfDictOfArray);
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

    deserializer.endObject();
}
